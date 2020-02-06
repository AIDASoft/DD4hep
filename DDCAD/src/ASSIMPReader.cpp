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
#include <DDCAD/ASSIMPReader.h>

/// Open Asset Importer Library

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::cad;

/// Read input file
vector<unique_ptr<TGeoTessellated> >
ASSIMPReader::read(const string& source)  const  {
  vector<unique_ptr<TGeoTessellated> > result;
  unique_ptr<Assimp::Importer> importer = make_unique<Assimp::Importer>();
  int flags = aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_CalcTangentSpace;
  auto scene = importer->ReadFile( source.c_str(), flags);
  if ( !scene )  {
    except("ASSIMPReader","+++ FileNotFound: %s",source.c_str());
  }
  for (unsigned int index = 0; index < scene->mNumMeshes; index++)   {
    aiMesh* mesh = scene->mMeshes[index];
    if ( mesh->mNumFaces > 0 )   {
      auto name = mesh->mName.C_Str();
      TessellatedSolid shape(name, mesh->mNumFaces);
      const aiVector3D* v = mesh->mVertices;
      for(unsigned int i=0; i < mesh->mNumFaces; i++)  {
        const aiFace&     face = mesh->mFaces[i];
        const unsigned int* idx = face.mIndices;
        Tessellated::Vertex_t a(v[idx[0]].x, v[idx[0]].y, v[idx[0]].z); 
        Tessellated::Vertex_t b(v[idx[1]].x, v[idx[1]].y, v[idx[1]].z); 
        Tessellated::Vertex_t c(v[idx[2]].x, v[idx[2]].y, v[idx[2]].z); 
        shape->AddFacet(a,b,c);
#if 0
        if ( scene->HasMaterials() )   {
          const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
          printout(INFO,"ASSIMPReader","+++ Material: %p",mat);
        }
#endif
      }
      if ( shape->GetNfacets() > 2 )   {
        result.emplace_back(unique_ptr<TGeoTessellated>(shape.ptr()));
        continue;
      }
      delete shape.ptr();
    }
  }
  return result;
}

