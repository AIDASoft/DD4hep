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
#include <CsgOps.h>

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
  bool equals(Vertex const &lhs, Vertex const &rhs)  {
    constexpr double kTolerance = 1.e-32;
    return TMath::Abs(lhs[0] - rhs[0]) < kTolerance &&
      TMath::Abs(lhs[1] - rhs[1]) < kTolerance &&
      TMath::Abs(lhs[2] - rhs[2]) < kTolerance;
  }

  struct TessellateShape   {
  public:
    TessellateShape() = default;
    virtual ~TessellateShape() = default;
    RootCsg::TBaseMesh* make_mesh(TGeoShape* sh)  const;
    RootCsg::TBaseMesh* collect_composite(TGeoCompositeShape* sh)    const;
    std::unique_ptr<TGeoTessellated> build_mesh(int id, const std::string& name, TGeoShape* shape);
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
    return move(tes);
  }

  std::unique_ptr<TGeoTessellated> TessellateShape::build_mesh(int id, const std::string& name, TGeoShape* shape)      {
    auto mesh = std::unique_ptr<RootCsg::TBaseMesh>(this->make_mesh(shape));
    std::vector<Vertex> vertices;
    std::size_t nskip = 0;

    vertices.reserve(mesh->NumberOfVertices());
    std::map<std::size_t,std::size_t> vtx_index_replacements;
    for( size_t ipoint = 0, npoints = mesh->NumberOfVertices(); ipoint < npoints; ++ipoint )   {
      long found = -1;
      const double* v = mesh->GetVertex(ipoint);
      Vertex vtx(v[0], v[1], v[2]);
      for(std::size_t i=0; i < vertices.size(); ++i)   {
        if ( equals(vertices[i],vtx) )  {
          vtx_index_replacements[ipoint] = found = i;
          break;
        }
      }
      if ( found < 0 )   {
        vtx_index_replacements[ipoint] = vertices.size();
        vertices.emplace_back(v[0], v[1], v[2]);
      }
    }
    std::size_t vtx_len = vertices.size();
    std::unique_ptr<TGeoTessellated> tes = std::make_unique<TGeoTessellated>(name.c_str(), vertices);
    for( std::size_t ipoly = 0, npols = mesh->NumberOfPolys(); ipoly < npols; ++ipoly)    {
      std::size_t npoints = mesh->SizeOfPoly(ipoly);
      if ( npoints >= 3 )   {
        printout(dd4hep::DEBUG,"ASSIMPWriter","+++ Got polygon with %ld points",npoints);
        ///
        /// 3-vertex polygons automatically translate to GL_TRIANGLES
        /// See Kronos documentation to glBegin / glEnd from the glu library:
        /// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glBegin.xml
        ///
        /// Otherwise:
#if 1
        /// Apparently this is the correct choice:
        ///
        /// Interprete as FAN:  GL_TRIANGLE_FAN
        /// One triangle is defined for each vertex presented after the first two vertices.
        /// Vertices 1 , n + 1 , and n + 2 define triangle n.
        /// N - 2 triangles are drawn.
        std::size_t v0  = mesh->GetVertexIndex(ipoly, 0);
        std::size_t vv0 = vtx_index_replacements[v0];
        for( std::size_t ipoint = 0; ipoint < npoints-2; ++ipoint )   {
          std::size_t v1 = mesh->GetVertexIndex(ipoly, ipoint+1);
          std::size_t v2 = mesh->GetVertexIndex(ipoly, ipoint+2);
          std::size_t vv1 = vtx_index_replacements[v1];
          std::size_t vv2 = vtx_index_replacements[v2];
          if ( vv0 > vtx_len || vv1 > vtx_len || vv2 > vtx_len )  {
            ++nskip;
            continue;
          }
          if ( vv0 == vv1 || vv0 == vv2 || vv1 == vv2 )   {
            ++nskip;
            continue;
          }
          Vertex w[3] = {vertices[vv0],vertices[vv1],vertices[vv2]};
          if ( TGeoFacet::CompactFacet(w, 3) < 3 )   {
            ++nskip;
            continue;
          }
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,31,1)
          bool degenerated = dd4hep::cad::facetIsDegenerated({vertices[vv0],vertices[vv1],vertices[vv2]});
#else
          bool degenerated = true;
          TGeoFacet f(&vertices, 3, vv0, vv1, vv2);
          f.ComputeNormal(degenerated);
#endif
          if ( degenerated )    {
            ++nskip;
            continue;
          }
          tes->AddFacet(vv0, vv1, vv2);
        }
#else
        /// Interprete as STRIP: GL_TRIANGLE_STRIP
        /// One triangle is defined for each vertex presented after the first two vertices.
        /// For odd n, vertices n, n + 1 , and n + 2 define triangle n.
        /// For even n, vertices n + 1 , n, and n + 2 define triangle n.
        /// N - 2 triangles are drawn.
        for( std::size_t ipoint = 0; ipoint < npoints-2; ++ipoint )   {
          vtx_t v0(mesh->GetVertex(mesh->GetVertexIndex(ipoly, ipoint)));
          vtx_t v1(mesh->GetVertex(mesh->GetVertexIndex(ipoly, ipoint+1)));
          vtx_t v2(mesh->GetVertex(mesh->GetVertexIndex(ipoly, ipoint+2)));
          ((ipoint%2) == 0) ? tes->AddFacet(v1, v0, v2) : tes->AddFacet(v0, v1, v2);
        }
#endif
      }
    }
    return close_tessellated(id, shape, nskip, move(tes));
  }
  
  RootCsg::TBaseMesh* TessellateShape::make_mesh(TGeoShape* sh)   const   {
    if (TGeoCompositeShape *shape = dynamic_cast<TGeoCompositeShape *>(sh))   {
      return collect_composite(shape);
    }
    UInt_t  flags = TBuffer3D::kCore|TBuffer3D::kBoundingBox|TBuffer3D::kRawSizes|TBuffer3D::kRaw|TBuffer3D::kShapeSpecific;
    const TBuffer3D& buffer = sh->GetBuffer3D(flags, kFALSE);
    return RootCsg::ConvertToMesh(buffer);
  }
  
  RootCsg::TBaseMesh* TessellateShape::collect_composite(TGeoCompositeShape* sh)  const  {
    TGeoBoolNode* node  = sh->GetBoolNode();
    TGeoShape*    left  = node->GetLeftShape();
    TGeoShape*    right = node->GetRightShape();
    TGeoHMatrix*  glmat = (TGeoHMatrix*)TGeoShape::GetTransform();
    UInt_t        oper  = node->GetBooleanOperator();
    TGeoHMatrix   copy(*glmat); // keep a copy

    // Do not wonder about this logic.
    // GetBuffer3D (->make_mesh) uses static variable fgTransform of TGeoShape!
    glmat->Multiply(node->GetLeftMatrix());
    auto left_mesh  = std::unique_ptr<RootCsg::TBaseMesh>(make_mesh(left));
    *glmat = &copy;

    glmat->Multiply(node->GetRightMatrix());
    auto right_mesh = std::unique_ptr<RootCsg::TBaseMesh>(make_mesh(right));
    *glmat = &copy;

    switch (oper) {
    case TGeoBoolNode::kGeoUnion:
      return RootCsg::BuildUnion(left_mesh.get(), right_mesh.get());
    case TGeoBoolNode::kGeoIntersection:
      return RootCsg::BuildIntersection(left_mesh.get(), right_mesh.get());
    case TGeoBoolNode::kGeoSubtraction:
      return RootCsg::BuildDifference(left_mesh.get(), right_mesh.get());
    default:
      Error("BuildComposite", "Wrong boolean operation code %d\n", oper);
      return 0;
    }
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
        shape_holder = helper.build_mesh(imesh, vol.name(), sol.ptr());
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
      mesh->mColors[0] = new aiColor4D(cr, cg, cb, ca);
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
