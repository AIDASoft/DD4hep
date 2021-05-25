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
#include <DD4hep/Shapes.h>
#include <DD4hep/Objects.h>
#include <DD4hep/Printout.h>
#include <DDCAD/ASSIMPWriter.h>

/// Open Asset Importer Library
#include "assimp/postprocess.h"
#include "assimp/Exporter.hpp"
#include "assimp/scene.h"

#include <TBuffer3D.h>
#include <TGeoMatrix.h>

/// C/C++ include files
#include <set>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cad;

namespace  {

  void _collect(std::vector<std::pair<PlacedVolume,TGeoHMatrix*> >& cont,
		bool recursive, const TGeoHMatrix& to_global, PlacedVolume pv)
  {
    Volume v = pv.volume();
    for(Int_t i=0; i<v->GetNdaughters(); ++i)  {
      PlacedVolume p = v->GetNode(i);
      Solid sol = p.volume().solid();
      // TessellatedSolid sol = p.volume().solid();
      // if ( sol.isValid() ) cont.push_back(p);
      unique_ptr<TGeoHMatrix> mother(new TGeoHMatrix(to_global));
      mother->Multiply(p->GetMatrix());

      if ( sol->IsA() != TGeoShapeAssembly::Class() )
	cont.push_back(make_pair(p, mother.get()));
      if ( recursive )
	_collect(cont, recursive, *mother, p);	
      if ( sol->IsA() != TGeoShapeAssembly::Class() )
	mother.release();
    }
  }
  struct vertex{ double x,y,z;
    vertex() = default;
    vertex(const vertex& copy) = default;
    vertex(double vx, double vy, double vz) : x(vx), y(vy), z(vz) {}
    vertex(const Tessellated::Vertex_t& v) : x(v.x()), y(v.y()), z(v.z()) {}
    vertex& operator=(const vertex& v) = default;
    vertex& operator=(const Tessellated::Vertex_t& v)
    { x = v.x(); y = v.y(); z = v.z(); return *this;  }
    operator Tessellated::Vertex_t () const
    { return Tessellated::Vertex_t(x,y,z); }
  };
#if 0
  vertex operator-(const vertex& v1, const vertex& v2)
  {  return vertex(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);   }
  vertex operator+(const vertex& v1, const vertex& v2)
  {  return vertex(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);   }
#endif
#if 0
      num_facet = tes->GetNfacets();
      int *nn = new int[num_facet];
      bool* flipped = new bool[num_facet];
      for( i=0; i < num_facet; ++i )
	flipped[i] = false, nn[0] = 0;
      for( i=0; i < num_facet; ++i )   {
	for( size_t j= i+1; j < num_facet; ++j)   {
	  bool isneighbour = tes->GetFacet(i).IsNeighbour(tes->GetFacet(j), flipped[j]);
	  if ( isneighbour )  {
	    if ( flipped[i] ) flipped[j] = !flipped[j];
	    ++nn[i];
	    ++nn[j];
	    if ( nn[i] == tes->GetFacet(i).GetNvert() )
	      break;
	  }
 	}
      }
      for( i=0; i < num_facet; ++i )  {
	if ( flipped[i] )  {
	  const_cast<TGeoFacet*>(&tes->GetFacet(i))->Flip();
	}
      }
      delete [] flipped;
      delete [] nn;
#endif
#if 0
   template <typename TGBinder>
   TPlane3 compute_plane(const TGBinder &poly)   {
      TPoint3 plast(poly[poly.Size()-1]);
      TPoint3 pivot;
      TVector3 edge;
      Int_t j;
      for (j = 0; j < poly.Size(); j++) {
         pivot = poly[j];
         edge =  pivot - plast;
         if (!edge.FuzzyZero()) break;
      }
      for (; j < poly.Size(); j++) {
         TVector3 v2 = poly[j] - pivot;
         TVector3 v3 = edge.Cross(v2);
         if (!v3.FuzzyZero())
            return TPlane3(v3,pivot);
      }

      return TPlane3();
   }


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
	int num_points[3];
	pol_t* pol = (pol_t*)q;
	int s1 = pol->segs[0], s2 = pol->segs[1];
        int ends[4] = {segs[s1]._1,segs[s1]._2,segs[s2]._1,segs[s2]._2};

	q += (2+pol->n);
	if (ends[0] == ends[2]) {
	  numPnts[0] = ends[1];
	  numPnts[1] = ends[0];
	  numPnts[2] = ends[3];
	}
	else if (ends[0] == ends[3]) {
	  numPnts[0] = ends[1];
	  numPnts[1] = ends[0];
	  numPnts[2] = ends[2];
	}
	else if (ends[1] == ends[2]) {
	  numPnts[0] = ends[0];
	  numPnts[1] = ends[1];
	  numPnts[2] = ends[3];
	}
	else {
	  numPnts[0] = ends[0];
	  numPnts[1] = ends[1];
	  numPnts[2] = ends[2];
	}
	Int_t lastAdded = numPnts[2];
	for( int j=pol->n; j != 2; --j )   {
	  ends[0] = segs[pol->segs[j]]._1;
	  ends[1] = segs[pol->segs[j]]._2;
	  if (ends[0] == lastAdded) {
	    lastAdded = ends[1];
	  }
	  else  {
	    lastAdded = ends[0];
	  }
	}

  
#endif
}

/// Write output file
int ASSIMPWriter::write(const std::string& file_name,
			const std::string& file_type,
			const VolumePlacements& places,
			bool   recursive,
			double unit_scale)  const
{
  std::vector<std::pair<PlacedVolume,TGeoHMatrix*> > placements;
  vector<Material>        materials;
  TGeoHMatrix             toGlobal;
    
  for( auto pv : places )
    _collect(placements, recursive, toGlobal, pv);

  size_t num_mesh = placements.size();

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
  root->mMeshes       = 0;//new unsigned int[root->mNumMeshes];

  for( size_t imesh=0; imesh < num_mesh; ++imesh )   {
    unique_ptr<TGeoHMatrix> trafo(placements[imesh].second);
    PlacedVolume     pv  = placements[imesh].first;
    Volume           v   = pv.volume();
    Material         m   = v.material();
    TessellatedSolid tes = v.solid();
    aiString         node_name(v.name());

    //
    unique_ptr<TGeoTessellated> buf;
    if ( !tes.isValid() )   {
      typedef vertex vtx_t;
      unique_ptr<TBuffer3D> buf3D(v.solid()->MakeBuffer3D());
      struct pol_t { int c, n; int segs[1]; } *pol = nullptr;
      struct seg_t { int c, _1, _2;         };
      const  seg_t* segs = (seg_t*)buf3D->fSegs;
      const  vtx_t* vtcs = (vtx_t*)buf3D->fPnts;
      size_t i, num_facet = 0;
      const  Int_t* q;

      for( i=0, q=buf3D->fPols; i<buf3D->NbPols(); ++i, q += (2+pol->n))  {
	pol = (pol_t*)q;
	for( int j=0; j < pol->n-1; ++j ) num_facet += 2;
      }
      
      buf = make_unique<TGeoTessellated>(v.name(), num_facet);
      tes = buf.get();
      q = buf3D->fPols;
      for( i=0, q=buf3D->fPols; i<buf3D->NbPols(); ++i)  {
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
    }
    if ( tes->GetNfacets() == 0 )   {
      continue;
    }
    
    size_t num_vert = 0;
    for( long j=0, n=tes->GetNfacets(); j < n; ++j )
      num_vert += tes->GetFacet(j).GetNvert();

    size_t index = std::numeric_limits<size_t>::max();
    for( size_t j=0; j<materials.size(); ++j )  {
      if( materials[j] == m )   {
	index = j;
	break;
      }
    }
    if ( index > materials.size() )   {
      aiString name(m.name());
      auto* ai_mat = new aiMaterial();
      index = materials.size();
      materials.push_back(m);
      ai_mat->AddProperty(&name, AI_MATKEY_NAME);
      scene.mMaterials[scene.mNumMaterials] = ai_mat;
      ++scene.mNumMaterials;
    }
    aiMesh* mesh = new aiMesh;
    mesh->mName = node_name;
    mesh->mMaterialIndex = index;
    if ( v.visAttributes().isValid() )   {
      float r = 0e0, g = 0e0, b = 0e0, a = 0e0;
      v.visAttributes().argb(a, r, g, b);
      mesh->mColors[0] = new aiColor4D(r, g, b, a);
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
    vertex vtx, tmp, norm;
    for( long j=0, nvx=0, n=tes->GetNfacets(); j < n; ++j )  {
      bool degenerated  = false;
      const auto& facet = tes->GetFacet(j);
      tmp = facet.ComputeNormal(degenerated);
      if ( !degenerated && facet.GetNvert() > 0 )   {
	aiFace& face  = mesh->mFaces[mesh->mNumFaces];
	double  u     = unit_scale;

	face.mIndices = new unsigned int[facet.GetNvert()];
	trafo->LocalToMaster(&tmp.x, &norm.x);
	face.mNumIndices = 0;
	for( long k=0; k < facet.GetNvert(); ++k )  {
	  tmp = facet.GetVertex(k);
	  trafo->LocalToMaster(&tmp.x, &vtx.x);
	  face.mIndices[face.mNumIndices] = nvx;
	  mesh->mNormals[nvx]  = aiVector3D(norm.x, norm.y, norm.z);
	  mesh->mVertices[nvx] = aiVector3D(vtx.x*u,vtx.y*u,vtx.z*u);
	  ++mesh->mNumVertices;
	  ++face.mNumIndices;
	  ++nvx;
	}
	++mesh->mNumFaces;
      }
    }
    if ( imesh == 122585 )
      break;
    else if ( imesh == 122586 )
      mesh->mNumFaces = 0;
    else if ( imesh == 122587 )
      mesh->mNumFaces = 0;

    /// Check if we have here a valid mesh
    if ( 0 == mesh->mNumFaces || 0 == mesh->mNumVertices )    {
      delete [] mesh->mVertices;
      mesh->mNumVertices = 0;
      delete [] mesh->mNormals;
      delete [] mesh->mFaces;
      mesh->mNumFaces = 0;
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
  printout(ALWAYS,"ASSIMPWriter","+++ Analysed %ld of %ld meshes.",
	   scene.mNumMeshes, placements.size());
  if ( scene.mNumMeshes > 0 )   {
    Assimp::Exporter exporter;
    Assimp::ExportProperties *props = new Assimp::ExportProperties;
    props->SetPropertyBool(AI_CONFIG_EXPORT_POINT_CLOUDS,
			   flags&EXPORT_POINT_CLOUDS ? true : false);
    exporter.Export(&scene, file_type.c_str(), file_name.c_str(), 0, props);
    return 1;
  }
  return 0;
}
