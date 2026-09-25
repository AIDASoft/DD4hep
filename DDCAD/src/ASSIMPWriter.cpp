//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

/// Framework include files
#include <DD4hep/Detector.h>
#include <DD4hep/Printout.h>
#include <DDCAD/ASSIMPWriter.h>
#include <DDCAD/Utilities.h>

/// Open Asset Importer Library
#include "assimp/postprocess.h"
#include "assimp/Exporter.hpp"
#include "assimp/scene.h"

/// ROOT include files
#include <TBuffer3D.h>
#include <TBuffer3DTypes.h>
#include <TClass.h>
#include <TGeoBoolNode.h>
#include <TGeoMatrix.h>

/// C/C++ include files
#include <set>

using namespace dd4hep::cad;

using Vertex = Tessellated::Vertex_t;

namespace  {

  void _collect(std::vector<std::pair<dd4hep::PlacedVolume,TGeoHMatrix*> >& cont,
                bool recursive, const TGeoHMatrix& to_global, dd4hep::PlacedVolume pv)
  {
    dd4hep::Volume v = pv.volume();
    for(Int_t i=0; i<v->GetNdaughters(); ++i)  {
      dd4hep::PlacedVolume p   = v->GetNode(i);
      dd4hep::Solid        sol = p.volume().solid();
      bool                 use = sol->IsA() != TGeoShapeAssembly::Class();
      std::unique_ptr<TGeoHMatrix> mother(new TGeoHMatrix(to_global));
      mother->Multiply(p->GetMatrix());

      if ( use )  {
        TGeoHMatrix* m = mother.release();
        cont.push_back(std::make_pair(p, m));
        if ( recursive )
          _collect(cont, recursive, *m, p);
      }
      else if ( recursive )  {
        _collect(cont, recursive, *mother, p);
      }
    }
  }

  void collect_leaf_shapes(TGeoShape* shape,
                           const TGeoHMatrix& parent_matrix,
                           std::vector<std::pair<TGeoShape*, TGeoHMatrix>>& leaf_shapes)
  {
    if (!shape) return;

    // Check if current shape is a boolean composite
    auto* composite = dynamic_cast<TGeoCompositeShape*>(shape);
    if (!composite) {
      // Primitive leaf shape found; record it along with its accumulated matrix
      leaf_shapes.push_back({shape, parent_matrix});
      return;
    }

    // Get the boolean node (union, subtraction, or intersection)
    TGeoBoolNode* node = composite->GetBoolNode();
    if (!node) return;

    // --- Process Left Branch ---
    TGeoShape* left_shape = node->GetLeftShape();
    if (left_shape) {
      TGeoHMatrix left_matrix(parent_matrix);
      if (TGeoMatrix* lm = node->GetLeftMatrix()) {
        left_matrix.Multiply(lm);
      }
      collect_leaf_shapes(left_shape, left_matrix, leaf_shapes);
    }

    // --- Process Right Branch ---
    TGeoShape* right_shape = node->GetRightShape();
    if (right_shape) {
      TGeoHMatrix right_matrix(parent_matrix);
      if (TGeoMatrix* rm = node->GetRightMatrix()) {
        right_matrix.Multiply(rm);
      }
      collect_leaf_shapes(right_shape, right_matrix, leaf_shapes);
    }
  }

  struct TessellateShape   {
  public:
    TessellateShape() = default;
    virtual ~TessellateShape() = default;
    std::unique_ptr<TGeoTessellated> make_mesh(TGeoShape* sh)  const;
    std::unique_ptr<TGeoTessellated> build_mesh(int id, TGeoShape* shape);
    std::unique_ptr<TGeoTessellated> close_tessellated(int id, TGeoShape* shape, int nskip, std::unique_ptr<TGeoTessellated>&& tes);
  };

  std::unique_ptr<TGeoTessellated>
  TessellateShape::close_tessellated(int id, TGeoShape* shape, int nskip, std::unique_ptr<TGeoTessellated>&& tes)   {

    if(not shape || not tes) {
      return nullptr;
    }

    std::string nam = shape->GetName(), typ = "["+std::string(shape->IsA()->GetName())+"]";
    nam = nam.substr(0, nam.find("_0x"));

    if (tes->GetNfacets() < 4) {
      std::cout << "ASSIMPWriter: Skipping degenerate shape with "
                << tes->GetNfacets() << " facets: " << shape->GetName() << std::endl;
      return nullptr;
    }

    tes->CloseShape(true, true, true);

    if ( nskip > 0 )   {
      dd4hep::printout(dd4hep::ALWAYS,
                       "ASSIMPWriter","+++ %3d %-48s %-24s Skipped %3ld/%-4d degenerate facets %4d vertices closed:%s defined:%s",
                       id, nam.c_str(), typ.c_str(), nskip, tes->GetNfacets(),  tes->GetNvertices(),
                       dd4hep::yes_no(tes->IsClosedBody()), dd4hep::yes_no(tes->IsDefined()));
    }
    else   {
      dd4hep::printout(dd4hep::ALWAYS,
                       "ASSIMPWriter","+++ %3d %-48s %-24s Tessellated %4d facets %4d vertices closed:%s defined:%s",
                       id, nam.c_str(), typ.c_str(),
                       tes->GetNfacets(), tes->GetNvertices(),
                       dd4hep::yes_no(tes->IsClosedBody()),
                       dd4hep::yes_no(tes->IsDefined()));
    }
    std::cout << std::flush;
    return std::move(tes);
  }


  std::unique_ptr<TGeoTessellated> TessellateShape::make_mesh(TGeoShape* sh) const {
    if (!sh) return nullptr;

    // Step 1: Request core info and raw sizes so ROOT computes NbPnts and resizes buffer.fPnts
    UInt_t size_flags = TBuffer3D::kCore | TBuffer3D::kBoundingBox | TBuffer3D::kShapeSpecific | TBuffer3D::kRawSizes;
    const TBuffer3D& buffer = sh->GetBuffer3D(size_flags, kFALSE);

    if (buffer.NbPnts() == 0 || buffer.NbPols() == 0) {
      return nullptr;
    }

    // Step 2: Request raw mesh data now that buffer.fPnts is allocated to fit buffer.NbPnts()
    sh->GetBuffer3D(TBuffer3D::kRaw | size_flags, kFALSE);

    if (!buffer.fPnts || !buffer.fPols || !buffer.fSegs) {
      return nullptr;
    }

    std::vector<Vertex> vertices;
    vertices.reserve(buffer.NbPnts());
    for (UInt_t i = 0; i < buffer.NbPnts(); ++i) {
      vertices.emplace_back(buffer.fPnts[3 * i], buffer.fPnts[3 * i + 1], buffer.fPnts[3 * i + 2]);
    }

    const char* shape_name = sh->GetName() ? sh->GetName() : "";

    // 1. Estimate number of facets
    std::size_t num_facets = 0;
    const Int_t* pols = buffer.fPols;
    Int_t idx = 0;
    for (UInt_t i = 0; i < buffer.NbPols(); ++i) {
      idx++; // skip color
      Int_t nsegs = pols[idx++];
      if (nsegs >= 3) {
        num_facets += (nsegs - 2);
      }
      idx += nsegs;
    }

    // 2. Initialize TGeoTessellated using facet count capacity (allows automatic vertex merging)
    auto tes = std::make_unique<TGeoTessellated>(shape_name, num_facets);

    const Int_t* segs = buffer.fSegs;
    idx = 0;

    for (UInt_t i = 0; i < buffer.NbPols(); ++i) {
      [[maybe_unused]] Int_t color = pols[idx++];
      Int_t nsegs = pols[idx++];

      if (nsegs >= 3) {
        std::vector<Int_t> poly_verts;
        poly_verts.reserve(nsegs);

        Int_t seg0_idx = pols[idx];
        Int_t v_start  = segs[3 * seg0_idx + 1];
        Int_t v_next   = segs[3 * seg0_idx + 2];

        Int_t seg1_idx = pols[idx + 1];
        Int_t s1_v0    = segs[3 * seg1_idx + 1];
        Int_t s1_v1    = segs[3 * seg1_idx + 2];

        if (v_start == s1_v0 || v_start == s1_v1) {
          std::swap(v_start, v_next);
        }

        poly_verts.push_back(v_start);
        Int_t curr_v = v_next;

        for (Int_t j = 1; j < nsegs; ++j) {
          poly_verts.push_back(curr_v);
          Int_t seg_idx = pols[idx + j];
          Int_t sv0     = segs[3 * seg_idx + 1];
          Int_t sv1     = segs[3 * seg_idx + 2];
          curr_v        = (sv0 == curr_v) ? sv1 : sv0;
        }

        // 3. Add facets using 3D Vertex objects so ROOT performs topological distance tolerance checks
        // Helper lambda to check if a triangle is geometrically degenerate (zero area)
        auto is_degenerate = [](const Vertex& p0, const Vertex& p1, const Vertex& p2) {
          double ax = p1.x() - p0.x(), ay = p1.y() - p0.y(), az = p1.z() - p0.z();
          double bx = p2.x() - p0.x(), by = p2.y() - p0.y(), bz = p2.z() - p0.z();
          double cx = ay * bz - az * by;
          double cy = az * bx - ax * bz;
          double cz = ax * by - ay * bx;
          return (cx * cx + cy * cy + cz * cz) < 1e-12; // Area squared threshold
        };
        Int_t idx0 = poly_verts[0];
        for (size_t j = 1; j + 1 < poly_verts.size(); ++j) {
          Int_t idx1 = poly_verts[j];
          Int_t idx2 = poly_verts[j + 1];

          if (idx0 >= 0 && static_cast<size_t>(idx0) < vertices.size() &&
              idx1 >= 0 && static_cast<size_t>(idx1) < vertices.size() &&
              idx2 >= 0 && static_cast<size_t>(idx2) < vertices.size()) {

            const auto& v0 = vertices[idx0];
            const auto& v1 = vertices[idx1];
            const auto& v2 = vertices[idx2];

            // Only pass non-degenerate facets to ROOT
            if (!is_degenerate(v0, v1, v2)) {
              tes->AddFacet(v0, v1, v2);
            }
          }
        }
      }
      idx += nsegs;
    }

    if (tes->GetNfacets() == 0) {
      return nullptr;
    }

    return tes;
  }
  

  std::unique_ptr<TGeoTessellated> TessellateShape::build_mesh(int id, TGeoShape* shape) {
    auto tes = make_mesh(shape);
    if (!tes) {
      return nullptr;
    }
    auto ret =  close_tessellated(id, shape, 0, std::move(tes));
    return ret;
  }

}

/// Write output file
int ASSIMPWriter::write(const std::string& file_name,
                        const std::string& file_type,
                        const VolumePlacements& places,
                        bool   recursive,
                        double unit_scale)  const
{
  std::vector<std::pair<PlacedVolume,TGeoHMatrix*> > placements;
  bool dump_facets = ((flags&0x2) != 0);
  std::vector<Material>  materials;
  TGeoHMatrix            toGlobal;

  for( auto pv : places )
    _collect(placements, recursive, toGlobal, pv);

  std::size_t num_mesh = placements.size();

  aiScene scene;
  scene.mNumMaterials = 0;
  scene.mNumMeshes    = 0;

  aiNode *root        = new aiNode();
  scene.mRootNode     = root;
  root->mName.Set("<STL>");
  root->mNumMeshes    = 0;
  root->mNumChildren  = 0;
  root->mMeshes       = 0;
  auto* geo_transform = TGeoShape::GetTransform();

  std::vector<aiMesh*>     scene_meshes;
  std::vector<aiMaterial*> scene_materials;
  std::vector<aiNode*>     root_children;

  TGeoHMatrix identity;
  for( std::size_t imesh=0; imesh < num_mesh; ++imesh )   {
    std::unique_ptr<TGeoHMatrix>     base_trafo(placements[imesh].second);
    PlacedVolume     pv  = placements[imesh].first;
    Volume           vol = pv.volume();
    Solid            sol = vol.solid();
    Material         mat = vol.material();
    aiString         node_name(vol.name());

    identity.Clear();
    TGeoShape::SetTransform(&identity);


    // Decompose boolean composite shapes into leaf shapes
    std::vector<std::pair<TGeoShape*, TGeoHMatrix>> leaf_shapes;
    collect_leaf_shapes(sol.ptr(), identity, leaf_shapes);

    for (const auto& [leaf_shape, csg_matrix] : leaf_shapes) {
      std::unique_ptr<TGeoHMatrix> trafo(new TGeoHMatrix(*base_trafo));
      trafo->Multiply(&csg_matrix);

      std::unique_ptr<TGeoTessellated> shape_holder;
      TessellatedSolid tes = leaf_shape;

      /// If the leaf shape is not tessellated, construct its mesh
      if (!tes.isValid()) {
        TessellateShape helper;
        auto* paintVol = detector.manager().GetPaintVolume();
        detector.manager().SetPaintVolume(vol.ptr());
        shape_holder = helper.build_mesh(imesh, leaf_shape);
        detector.manager().SetPaintVolume(paintVol);
        if (!shape_holder) {
          continue;
        }
        tes = shape_holder.get();
      }

      if (tes->GetNfacets() == 0) {
        continue;
      }

      std::size_t num_vert = 0;
      for( long j=0, n=tes->GetNfacets(); j < n; ++j )
        num_vert += tes->GetFacet(j).GetNvert();

      std::size_t index = std::numeric_limits<std::size_t>::max();
      for( std::size_t j=0; j<materials.size(); ++j )  {
        if( materials[j] == mat )   {
          index = j;
          break;
        }
      }
      if ( index > materials.size() )   {
        aiString name(mat.name());
        auto* ai_mat = new aiMaterial();
        index = materials.size();
        materials.push_back(mat);
        ai_mat->AddProperty(&name, AI_MATKEY_NAME);
        scene_materials.push_back(ai_mat);
      }
      aiMesh* mesh = new aiMesh;
      mesh->mName = node_name;
      mesh->mMaterialIndex = index;
      if ( vol.visAttributes().isValid() )   {
        float cr = 0e0, cg = 0e0, cb = 0e0, ca = 0e0;
        vol.visAttributes().argb(ca, cr, cg, cb);

        mesh->mColors[0] = new aiColor4D[num_vert];
        for(std::size_t k = 0; k < num_vert; ++k) {
          mesh->mColors[0][k] = aiColor4D(cr, cg, cb, ca);
        }
      }
      mesh->mFaces       = new aiFace[tes->GetNfacets()];
      mesh->mVertices    = new aiVector3D[num_vert];
      mesh->mNormals     = new aiVector3D[num_vert];
      mesh->mTangents    = nullptr;
      mesh->mBitangents  = nullptr;
      mesh->mNumFaces    = 0;
      mesh->mNumVertices = 0;

      for( long j=0, n=tes->GetNfacets(); j < n; ++j )   {
        aiFace& face = mesh->mFaces[j];
        face.mNumIndices = 0;
        face.mIndices = nullptr;
      }
      Vertex vtx, tmp, norm;
      for( long j=0, nvx=0, n=tes->GetNfacets(); j < n; ++j )  {
        bool degenerated  = false;
        const auto& facet = tes->GetFacet(j);
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,31,1)
        tmp = tes->FacetComputeNormal(j, degenerated);
#else
        tmp = facet.ComputeNormal(degenerated);
#endif
        if ( !degenerated && facet.GetNvert() > 0 )   {
          aiFace& face  = mesh->mFaces[mesh->mNumFaces];
          double  u     = unit_scale;

          face.mIndices = new unsigned int[facet.GetNvert()];
          trafo->LocalToMaster(tmp.fVec, norm.fVec);
          face.mNumIndices = 0;
          for( long k=0; k < facet.GetNvert(); ++k )  {
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,31,1)
            tmp = tes->GetVertex(facet[k]);
#else
            tmp = facet.GetVertex(k);
#endif
            trafo->LocalToMaster(tmp.fVec, vtx.fVec);
            face.mIndices[face.mNumIndices] = nvx;
            mesh->mNormals[nvx]  = aiVector3D(norm.x(), norm.y(), norm.z());
            mesh->mVertices[nvx] = aiVector3D(vtx.x()*u,vtx.y()*u,vtx.z()*u);
            ++mesh->mNumVertices;
            ++face.mNumIndices;
            ++nvx;
          }
          ++mesh->mNumFaces;
          if ( dump_facets )   {
            const auto* id = face.mIndices;
            const auto* vv = mesh->mVertices;
            ROOT::Geom::Vertex_t v1(vv[id[0]].x, vv[id[0]].y, vv[id[0]].z);
            ROOT::Geom::Vertex_t v2(vv[id[1]].x, vv[id[1]].y, vv[id[1]].z);
            ROOT::Geom::Vertex_t v3(vv[id[2]].x, vv[id[2]].y, vv[id[2]].z);
            std::string str = dd4hep::cad::streamVertices(v1, v2, v3);
            printout(ALWAYS,"ASSIMPWriter","++ Facet %4ld : %s", j, str.c_str());
          }
        }
        else   {
          printout(ALWAYS,"ASSIMPWriter",
                   "+++ Found degenerate facet while writing [Should not happen]");
        }
      }
    
      /// Check if we have here a valid mesh
      if ( 0 == mesh->mNumFaces || 0 == mesh->mNumVertices )    {
        if ( mesh->mVertices ) delete [] mesh->mVertices;
        mesh->mVertices = nullptr;
        mesh->mNumVertices = 0;
        if ( mesh->mNormals ) delete [] mesh->mNormals;
        mesh->mNormals = nullptr;
        if ( mesh->mFaces ) delete [] mesh->mFaces;
        mesh->mFaces = nullptr;
        mesh->mNumFaces = 0;
        delete mesh;
        continue;
      }

      scene_meshes.push_back(mesh);

      aiNode *node      = new aiNode;
      node->mMeshes     = new unsigned int[node->mNumMeshes=1];
      node->mMeshes[0]  = scene_meshes.size() - 1;
      node->mParent     = root;
      node->mName.Set("<STL>");

      root_children.push_back(node);
    } // end leaf_shapes loop
  }

  // Assign dynamically sized arrays to scene structures
  scene.mNumMeshes = scene_meshes.size();
  if (scene.mNumMeshes > 0) {
    scene.mMeshes = new aiMesh*[scene.mNumMeshes];
    std::copy(scene_meshes.begin(), scene_meshes.end(), scene.mMeshes);
  }

  scene.mNumMaterials = scene_materials.size();
  if (scene.mNumMaterials > 0) {
    scene.mMaterials = new aiMaterial*[scene.mNumMaterials];
    std::copy(scene_materials.begin(), scene_materials.end(), scene.mMaterials);
  }

  root->mNumChildren = root_children.size();
  if (root->mNumChildren > 0) {
    root->mChildren = new aiNode*[root->mNumChildren];
    std::copy(root_children.begin(), root_children.end(), root->mChildren);
  }

  TGeoShape::SetTransform(geo_transform);
  printout(ALWAYS,"ASSIMPWriter","+++ Analysed %ld out of %ld meshes.",
           scene.mNumMeshes, placements.size());
  if ( scene.mNumMeshes > 0 )   {
    Assimp::Exporter exporter;
    Assimp::ExportProperties *props = new Assimp::ExportProperties;
    props->SetPropertyBool(AI_CONFIG_EXPORT_POINT_CLOUDS, flags&EXPORT_POINT_CLOUDS ? true : false);
    exporter.Export(&scene, file_type.c_str(), file_name.c_str(), 0, props);
    return 1;
  }
  return 0;
}
