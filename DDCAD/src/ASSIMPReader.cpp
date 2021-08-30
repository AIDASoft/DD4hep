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

/// C/C++ include files
#include <sstream>

using namespace dd4hep;
using namespace dd4hep::cad;

/// Read input file
std::vector<std::unique_ptr<TGeoTessellated> >
ASSIMPReader::readShapes(const std::string& source, double unit_length)  const
{
  typedef TessellatedSolid::Vertex Vertex;
  std::vector<std::unique_ptr<TGeoTessellated> > result;
  std::unique_ptr<Assimp::Importer> importer = std::make_unique<Assimp::Importer>();
  int aiflags = aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_CalcTangentSpace;
  auto scene = importer->ReadFile( source.c_str(), aiflags);
  if ( !scene )  {
    except("ASSIMPReader","+++ FileNotFound: %s",source.c_str());
  }
  double unit = unit_length;
  bool   dump_facets = ((flags>>8)&0x1) == 1;
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
        if ( dump_facets )   {
          size_t which = shape->GetNfacets()-1;
          const auto& facet = shape->GetFacet(which);
          std::stringstream str;
          str << facet;
          printout(ALWAYS,"ASSIMPReader","++ Facet %4ld : %s",
                   which, str.str().c_str());
        }
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
  bool dump_facets = ((flags>>8)&0x1) == 1;
  int aiflags = aiProcess_Triangulate|aiProcess_JoinIdenticalVertices|aiProcess_CalcTangentSpace;
  auto scene = importer->ReadFile( source.c_str(), aiflags);
  char text[1048];
  
  if ( !scene )  {
    except("ASSIMPReader","+++ FileNotFound: %s",source.c_str());
  }
  double unit = unit_length;
  for (unsigned int index = 0; index < scene->mNumMeshes; index++)   {
    aiMesh* mesh = scene->mMeshes[index];
    if ( mesh->mNumFaces > 0 )   {
      std::string name = mesh->mName.C_Str();
      std::vector<Vertex> vertices;
      const aiVector3D* v = mesh->mVertices;
      for(unsigned int i=0; i < mesh->mNumVertices; i++)  {
        vertices.emplace_back(Vertex(v[i].x*unit, v[i].y*unit, v[i].z*unit));
      }
      TessellatedSolid shape(name,vertices);
      for(unsigned int i=0; i < mesh->mNumFaces; i++)  {
        const unsigned int* idx  = mesh->mFaces[i].mIndices;
        shape->AddFacet(idx[0], idx[1], idx[2]);
      }
#if 0
      TessellatedSolid shape(name, mesh->mNumFaces);
      for(unsigned int i=0; i < mesh->mNumFaces; i++)  {
        const aiFace&     face = mesh->mFaces[i];
        const unsigned int* idx = face.mIndices;
        Vertex a(v[idx[0]].x*unit, v[idx[0]].y*unit, v[idx[0]].z*unit); 
        Vertex b(v[idx[1]].x*unit, v[idx[1]].y*unit, v[idx[1]].z*unit); 
        Vertex c(v[idx[2]].x*unit, v[idx[2]].y*unit, v[idx[2]].z*unit); 
        shape->AddFacet(a,b,c);
      }
#endif
      if ( shape->GetNfacets() > 2 )   {
        std::string mat_name;
        Material mat;
        VisAttr  vis;
        if ( scene->HasMaterials() )   {
          aiMaterial* ai_mat = scene->mMaterials[mesh->mMaterialIndex];
          mat_name = ai_mat->GetName().C_Str();
          mat = detector.material(mat_name);
        }
        if ( !mat.isValid() )   {
          printout(ERROR, "ASSIMPReader",
                   "+++ %s: No material named '%s' FOUND. Will use Air. [Missing material]",
                   text, mat_name.c_str());
          mat = detector.air();
        }
        if ( name.empty() )  {
          ::snprintf(text,sizeof(text),"tessellated_%ld", result.size());
          text[sizeof(text)-1] = 0;
          name = text;
        }
        Volume vol(name, Solid(shape.ptr()), mat);
        if ( mesh->HasVertexColors(0) )   {
          const aiColor4D* col = mesh->mColors[0];
          if ( col )   {
            for( const auto& _v : detector.visAttributes() )   {
              float ca, cr, cg, cb, eps = 0.05;
              VisAttr(_v.second).argb(ca, cr, cg, cb);
              if( std::abs(col->a-ca) < eps && std::abs(col->r-cr) < eps &&
                  std::abs(col->g-cg) < eps && std::abs(col->b-cb) < eps )   {
                vis = _v.second;
                break;
              }
            }
            if ( !vis.isValid() )   {
              ::snprintf(text,sizeof(text),"vis_%s_%p", name.c_str(), (void*)vol.ptr());
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
        shape->CloseShape(true,true,true);
        if ( dump_facets )   {
          for( size_t i=0, n=shape->GetNfacets(); i < n; ++i )   {
            const auto& facet = shape->GetFacet(i);
            std::stringstream str;
            str << facet;
            printout(ALWAYS,"ASSIMPReader","++ Facet %4ld : %s",
                     i, str.str().c_str());
          }
        }
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
