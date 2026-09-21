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

  struct TessellateShape   {
  public:
    TessellateShape() = default;
    virtual ~TessellateShape() = default;
    std::unique_ptr<TGeoTessellated> make_mesh(TGeoShape* sh)  const;
    std::unique_ptr<TGeoTessellated> collect_composite(TGeoCompositeShape* sh)    const;
    std::unique_ptr<TGeoTessellated> build_mesh(int id, TGeoShape* shape);
    std::unique_ptr<TGeoTessellated> tessellate_primitive(const std::string& name, dd4hep::Solid solid);
    std::unique_ptr<TGeoTessellated> close_tessellated(int id, TGeoShape* shape, int nskip, std::unique_ptr<TGeoTessellated>&& tes);
  };

  std::unique_ptr<TGeoTessellated>
  TessellateShape::close_tessellated(int id, TGeoShape* shape, int nskip, std::unique_ptr<TGeoTessellated>&& tes)   {
    std::string nam = shape->GetName(), typ = "["+std::string(shape->IsA()->GetName())+"]";
    nam = nam.substr(0, nam.find("_0x"));
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

    if (TGeoCompositeShape *shape = dynamic_cast<TGeoCompositeShape *>(sh)) {
      return collect_composite(shape);
    }

    UInt_t flags = TBuffer3D::kCore | TBuffer3D::kBoundingBox | TBuffer3D::kRawSizes | TBuffer3D::kRaw | TBuffer3D::kShapeSpecific;
    const TBuffer3D& buffer = sh->GetBuffer3D(flags, kFALSE);

    if (buffer.NbPnts() == 0 || buffer.NbPols() == 0 || !buffer.fPnts || !buffer.fPols || !buffer.fSegs) {
      return nullptr;
    }

    std::vector<Vertex> vertices;
    vertices.reserve(buffer.NbPnts());
    for (UInt_t i = 0; i < buffer.NbPnts(); ++i) {
      vertices.emplace_back(buffer.fPnts[3 * i], buffer.fPnts[3 * i + 1], buffer.fPnts[3 * i + 2]);
    }

    const char* shape_name = sh->GetName() ? sh->GetName() : "";
    auto tes = std::make_unique<TGeoTessellated>(shape_name, vertices);
    const size_t num_vertices = vertices.size();

    const Int_t* pols = buffer.fPols;
    const Int_t* segs = buffer.fSegs;
    Int_t idx = 0;

    for (UInt_t i = 0; i < buffer.NbPols(); ++i) {
      [[maybe_unused]] Int_t color = pols[idx++];
      Int_t nsegs = pols[idx++];

      if (nsegs >= 3) {
        std::vector<Int_t> poly_verts;
        poly_verts.reserve(nsegs);

        // Traverse segments in sequence while following ROOT TBuffer3D segment direction
        for (Int_t j = 0; j < nsegs; ++j) {
          Int_t seg_idx = pols[idx + j];
          Int_t v0 = segs[3 * seg_idx + 1];
          Int_t v1 = segs[3 * seg_idx + 2];

          if (j == 0) {
            // Determine loop orientation from second segment
            Int_t next_seg = pols[idx + 1];
            Int_t next_v0 = segs[3 * next_seg + 1];
            Int_t next_v1 = segs[3 * next_seg + 2];

            if (v1 == next_v0 || v1 == next_v1) {
              poly_verts.push_back(v0);
              poly_verts.push_back(v1);
            } else {
              poly_verts.push_back(v1);
              poly_verts.push_back(v0);
            }
          } else if (j < nsegs - 1) {
            Int_t last_v = poly_verts.back();
            Int_t next_v = (v0 == last_v) ? v1 : v0;
            poly_verts.push_back(next_v);
          }
        }

        // Triangulate n-gon face while preserving consistent CCW winding order
        Int_t v_start = poly_verts[0];
        for (size_t j = 1; j + 1 < poly_verts.size(); ++j) {
          Int_t v_mid = poly_verts[j];
          Int_t v_end = poly_verts[j + 1];

          if (v_start >= 0 && static_cast<size_t>(v_start) < num_vertices &&
              v_mid >= 0   && static_cast<size_t>(v_mid)   < num_vertices &&
              v_end >= 0   && static_cast<size_t>(v_end)   < num_vertices) {

            if (v_start != v_mid && v_start != v_end && v_mid != v_end) {
              tes->AddFacet(static_cast<UInt_t>(v_start),
                            static_cast<UInt_t>(v_mid),
                            static_cast<UInt_t>(v_end));
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
  
  std::unique_ptr<TGeoTessellated> TessellateShape::collect_composite(TGeoCompositeShape* sh) const {
    return make_mesh(static_cast<TGeoShape*>(sh));
  }

  std::unique_ptr<TGeoTessellated> TessellateShape::build_mesh(int id, TGeoShape* shape) {
    auto tes = make_mesh(shape);
    if (!tes) {
      // Create a non-empty fallback shape with 1 dummy facet (3 zero vertices)
      // to avoid triggering ROOT BVH assertion prim_count != 0
      const char* shape_name = (shape && shape->GetName()) ? shape->GetName() : "";
      std::vector<Vertex> dummy_vtx = { Vertex(0,0,0), Vertex(0,0,0), Vertex(0,0,0) };
      tes = std::make_unique<TGeoTessellated>(shape_name, dummy_vtx);
      tes->AddFacet(0, 1, 2);
    }

    return close_tessellated(id, shape, 0, std::move(tes));
  }

  std::unique_ptr<TGeoTessellated> TessellateShape::tessellate_primitive(const std::string& name, dd4hep::Solid solid)   {
    using  vtx_t = Vertex;
    const  TBuffer3D& buf3D = solid->GetBuffer3D(TBuffer3D::kAll, false);
    struct pol_t { int c, n; int segs[1]; } *pol = nullptr;
    struct seg_t { int c, _1, _2;         };
    const  seg_t* segs = (seg_t*)buf3D.fSegs;
    const  vtx_t* vtcs = (vtx_t*)buf3D.fPnts;
    std::size_t i, num_facet = 0;
    const  Int_t* q;

    for( i=0, q=buf3D.fPols; i<buf3D.NbPols(); ++i, q += (2+pol->n))  {
      pol = (pol_t*)q;
      for( int j=0; j < pol->n-1; ++j ) num_facet += 2;
    }

    std::unique_ptr<TGeoTessellated> tes = std::make_unique<TGeoTessellated>(name.c_str(), num_facet);
    q = buf3D.fPols;
    for( i=0, q=buf3D.fPols; i<buf3D.NbPols(); ++i)  {
      pol = (pol_t*)q;
      q += (2+pol->n);
      for( int j=0; j < pol->n; j += 2 )   {
        /* ------------------------------------------------------------
        //   Convert quadri-linear facet to 2 tri-linear facets
        //
        //    f1 +---------------+ v2/v3: f0
        //      /                / 
        //     /                /
        //    /                /
        //    +---------------+
        //  v0             v1 v2/v3
        // --------------------------------------------------------- */
        const int    s1  = pol->segs[j], s2 = pol->segs[(j+1)%pol->n];
        const int    s[] = { segs[s1]._1, segs[s1]._2, segs[s2]._1, segs[s2]._2 };
        const vtx_t& v0  = vtcs[s[0]], &v1=vtcs[s[1]], &v2=vtcs[s[2]], &v3=vtcs[s[3]];

        if ( s[0] == s[2] )   {       // Points are ( s[1], s[0], s[3] )
          tes->AddFacet(v1, v0, v3);
        }
        else if ( s[0] == s[3] )   {  // Points are ( s[1], s[0], s[2] )
          tes->AddFacet(v1, v0, v2);
        }
        else if ( s[1] == s[2] )   {  // Points are ( s[0], s[1], s[3] )
          tes->AddFacet(v0, v1, v3);
        }
        else   {                      // Points are ( s[0], s[1], s[2] )
          tes->AddFacet(v0, v1, v2);
        }
      }
    }
    return tes;
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
  int  build_mode  = ((flags&0x1) != 0) ? 1 : 0;
  bool dump_facets = ((flags&0x2) != 0);
  std::vector<Material>  materials;
  TGeoHMatrix            toGlobal;

  for( auto pv : places )
    _collect(placements, recursive, toGlobal, pv);

  std::size_t num_mesh = placements.size();

  aiScene scene;
  scene.mNumMaterials = 0;
  scene.mNumMeshes    = 0;
  scene.mMeshes       = new aiMesh* [num_mesh];
  scene.mMaterials    = new aiMaterial* [num_mesh];

  aiNode *root        = new aiNode();
  scene.mRootNode     = root;
  root->mName.Set("<STL>");
  root->mNumMeshes    = 0;
  root->mNumChildren  = 0;
  root->mChildren     = new aiNode* [num_mesh];
  root->mMeshes       = 0;
  auto* geo_transform = TGeoShape::GetTransform();

  TGeoHMatrix identity;
  for( std::size_t imesh=0; imesh < num_mesh; ++imesh )   {
    std::unique_ptr<TGeoHMatrix>     trafo(placements[imesh].second);
    std::unique_ptr<TGeoTessellated> shape_holder;
    PlacedVolume     pv  = placements[imesh].first;
    Volume           vol = pv.volume();
    Solid            sol = vol.solid();
    Material         mat = vol.material();
    TessellatedSolid tes = sol;
    aiString         node_name(vol.name());

    identity.Clear();
    TGeoShape::SetTransform(&identity);

    /// If the shape is already tessellated, no need to create another one!
    if ( !tes.isValid() )   {
      TessellateShape helper;
      auto* shape = dynamic_cast<TGeoCompositeShape*>(sol.ptr());
      if ( build_mode || shape )   {  // Always use this method!
        auto* paintVol = detector.manager().GetPaintVolume();
        detector.manager().SetPaintVolume(vol.ptr());
        shape_holder = helper.build_mesh(imesh, sol.ptr());
        detector.manager().SetPaintVolume(paintVol);
      }
      else   {
        shape_holder = helper.tessellate_primitive(vol.name(), sol);
      }
      tes = shape_holder.get();
    }
    if ( tes->GetNfacets() == 0 )   {
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
      scene.mMaterials[scene.mNumMaterials] = ai_mat;
      ++scene.mNumMaterials;
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
    
    scene.mMeshes[scene.mNumMeshes] = mesh;

    aiNode *node      = new aiNode;
    node->mMeshes     = new unsigned int[node->mNumMeshes=1];
    node->mMeshes[0]  = scene.mNumMeshes;
    node->mParent     = root;
    node->mName.Set("<STL>");

    root->mChildren[root->mNumChildren] = node;
    ++root->mNumChildren;
    ++scene.mNumMeshes;
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
