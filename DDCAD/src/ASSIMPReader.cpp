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
#include <DD4hep/Printout.h>
#include <DD4hep/Detector.h>
#include <DDCAD/ASSIMPReader.h>

/// Open Asset Importer Library

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace dd4hep;
using namespace dd4hep::cad;

/// Read input file
std::vector<std::unique_ptr<TGeoTessellated> >
ASSIMPReader::readShapes(const std::string& source, double unit_length)  const
{
  typedef TessellatedSolid::Vertex Vertex;
  std::vector<std::unique_ptr<TGeoTessellated> > result;
  std::unique_ptr<Assimp::Importer> importer = std::make_unique<Assimp::Importer>();
  int flags = aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_CalcTangentSpace;
  auto scene = importer->ReadFile( source.c_str(), flags);
  if ( !scene )  {
    except("ASSIMPReader","+++ FileNotFound: %s",source.c_str());
  }
  double unit = unit_length;
  for (unsigned int index = 0; index < scene->mNumMeshes; index++)   {
    aiMesh* mesh = scene->mMeshes[index];
    if ( mesh->mNumFaces > 0 )   {
      auto name = mesh->mName.C_Str();
      TessellatedSolid shape(name, mesh->mNumFaces);
      const aiVector3D* v = mesh->mVertices;
      for(unsigned int i=0; i < mesh->mNumFaces; i++)  {
        const aiFace&     face = mesh->mFaces[i];
        const unsigned int* idx = face.mIndices;
        Vertex a(v[idx[0]].x*unit, v[idx[0]].y*unit, v[idx[0]].z*unit); 
        Vertex b(v[idx[1]].x*unit, v[idx[1]].y*unit, v[idx[1]].z*unit); 
        Vertex c(v[idx[2]].x*unit, v[idx[2]].y*unit, v[idx[2]].z*unit); 
        shape->AddFacet(a,b,c);
      }
      if ( shape->GetNfacets() > 2 )   {
        result.emplace_back(std::unique_ptr<TGeoTessellated>(shape.ptr()));
        continue;
      }
      delete shape.ptr();
    }
  }
  printout(ALWAYS,"ASSIMPReader","+++ Read %ld meshes from %s",
	   result.size(), source.c_str());
  return result;
}

/// Read input file and create a volume-set
std::vector<std::unique_ptr<TGeoVolume> >
ASSIMPReader::readVolumes(const std::string& source, double unit_length)  const
{
  typedef TessellatedSolid::Vertex Vertex;
  std::vector<std::unique_ptr<TGeoVolume> > result;
  std::unique_ptr<Assimp::Importer> importer = std::make_unique<Assimp::Importer>();
  int flags = aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_CalcTangentSpace;
  auto scene = importer->ReadFile( source.c_str(), flags);
  char text[128];
  
  if ( !scene )  {
    except("ASSIMPReader","+++ FileNotFound: %s",source.c_str());
  }
  double unit = unit_length;
  for (unsigned int index = 0; index < scene->mNumMeshes; index++)   {
    aiMesh* mesh = scene->mMeshes[index];
    if ( mesh->mNumFaces > 0 )   {
      auto name = mesh->mName.C_Str();
      TessellatedSolid shape(name, mesh->mNumFaces);
      const aiVector3D* v = mesh->mVertices;
      for(unsigned int i=0; i < mesh->mNumFaces; i++)  {
        const aiFace&     face = mesh->mFaces[i];
        const unsigned int* idx = face.mIndices;
        Vertex a(v[idx[0]].x*unit, v[idx[0]].y*unit, v[idx[0]].z*unit); 
        Vertex b(v[idx[1]].x*unit, v[idx[1]].y*unit, v[idx[1]].z*unit); 
        Vertex c(v[idx[2]].x*unit, v[idx[2]].y*unit, v[idx[2]].z*unit); 
        shape->AddFacet(a,b,c);
      }
      if ( shape->GetNfacets() > 2 )   {
	std::string nam;
	Material mat;
	VisAttr  vis;
        if ( scene->HasMaterials() )   {
          aiMaterial* ai_mat = scene->mMaterials[mesh->mMaterialIndex];
	  nam = ai_mat->GetName().C_Str();
	  mat = detector.material(nam);
        }
	if ( !mat.isValid() )   {
	  printout(ERROR, "ASSIMPReader",
		   "+++ %s: No material named '%s' FOUND. Will use Air. [Missing material]",
		   text, nam.c_str());
 	  mat = detector.air();
	}
	::snprintf(text,sizeof(text),"tessellated_%ld", result.size());
	text[sizeof(text)-1] = 0;
	Volume vol(text, Solid(shape.ptr()), mat);
	if ( mesh->HasVertexColors(0) )   {
	  const aiColor4D* col = mesh->mColors[0];
	  if ( col )   {
	    for( const auto& _v : detector.visAttributes() )   {
	      float a, r, g, b, eps = 0.05;
	      VisAttr(_v.second).argb(a, r, g, b);
	      if( std::abs(col->a-a) < eps && std::abs(col->r-r) < eps &&
		  std::abs(col->g-g) < eps && std::abs(col->b-b) < eps )   {
		vis = _v.second;
		break;
	      }
	    }
	    if ( !vis.isValid() )   {
	      ::snprintf(text,sizeof(text),"vis_tessellated_%p", (void*)vol.ptr());
	      text[sizeof(text)-1] = 0;
	      vis = VisAttr(text);
	      vis.setColor(col->a,col->r,col->g,col->b);
	      detector.add(vis);
	    }
	    vol.setVisAttributes(vis);
	  }
	}
	printout(INFO,"ASSIMPReader",
		 "+++ %-17s Material: %-16s  Viualization: %s",
		 vol.name(), mat.name(), vis.isValid() ? vis.name() : "NONE");
        result.emplace_back(std::unique_ptr<TGeoVolume>(vol.ptr()));
        continue;
      }
      delete shape.ptr();
    }
  }
  printout(ALWAYS,"ASSIMPReader","+++ Read %ld meshes from %s",
	   result.size(), source.c_str());
  return result;
}
