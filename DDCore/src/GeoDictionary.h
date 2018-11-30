//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  RootDictionary.h
//
//
//  M.Frank
//
//==========================================================================
#ifndef DD4HEP_DDCORE_ROOTDICTIONARY_H
#define DD4HEP_DDCORE_ROOTDICTIONARY_H

// Framework include files
#include "DD4hep/Volumes.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/VolumeProcessor.h"

// C/C++ include files
#include <vector>
#include <map>
#include <string>

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#ifdef DD4HEP_DICTIONARY_MODE
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

using namespace std;

#pragma link C++ namespace dd4hep;

// Volume.h
#pragma link C++ class dd4hep::Volume+;
#pragma link C++ class dd4hep::VolumeExtension+;
#pragma link C++ class vector<dd4hep::Volume>+;
#pragma link C++ class dd4hep::Handle<TGeoVolume>+;

#pragma link C++ class dd4hep::PlacedVolume+;
#pragma link C++ class dd4hep::PlacedVolume::Processor+;

#ifndef __ROOTCLING__
template vector<pair<string, int> >;
template vector<pair<string, int> >::iterator;
#endif
#pragma link C++ class vector<pair<string, int> >+;
#pragma link C++ class vector<pair<string, int> >::iterator;
#pragma link C++ class dd4hep::PlacedVolumeExtension::VolIDs+;
#pragma link C++ class dd4hep::PlacedVolumeExtension+;
#pragma link C++ class vector<dd4hep::PlacedVolume>+;
#pragma link C++ class dd4hep::Handle<TGeoNode>+;
#pragma link C++ class vector<TGeoNode*>+;
#pragma link C++ class vector<TGeoVolume*>+;


// Shapes.h
#pragma link C++ class dd4hep::Handle<TGeoShape>+;
#pragma link C++ class dd4hep::Solid_type<TGeoShape>+;

#pragma link C++ class dd4hep::Polycone+;
#pragma link C++ class dd4hep::Solid_type<TGeoPcon>+;
#pragma link C++ class dd4hep::Handle<TGeoPcon>+;

#pragma link C++ class dd4hep::ConeSegment+;
#pragma link C++ class dd4hep::Solid_type<TGeoConeSeg>+;
#pragma link C++ class dd4hep::Handle<TGeoConeSeg>+;

#pragma link C++ class dd4hep::Box+;
#pragma link C++ class dd4hep::Solid_type<TGeoBBox>+;
#pragma link C++ class dd4hep::Handle<TGeoBBox>+;

#pragma link C++ class dd4hep::Torus+;
#pragma link C++ class dd4hep::Solid_type<TGeoTorus>+;
#pragma link C++ class dd4hep::Handle<TGeoTorus>+;

#pragma link C++ class dd4hep::Cone+;
#pragma link C++ class dd4hep::Solid_type<TGeoCone>+;
#pragma link C++ class dd4hep::Handle<TGeoCone>+;

#pragma link C++ class dd4hep::Tube+;
#pragma link C++ class dd4hep::Solid_type<TGeoTubeSeg>+;
#pragma link C++ class dd4hep::Handle<TGeoTubeSeg>+;

#pragma link C++ class dd4hep::EllipticalTube+;
#pragma link C++ class dd4hep::Solid_type<TGeoEltu>+;
#pragma link C++ class dd4hep::Handle<TGeoEltu>+;

#pragma link C++ class dd4hep::Trap+;
#pragma link C++ class dd4hep::Solid_type<TGeoTrap>+;
#pragma link C++ class dd4hep::Handle<TGeoTrap>+;

#pragma link C++ class dd4hep::Trd1+;
#pragma link C++ class dd4hep::Solid_type<TGeoTrd2>+;
#pragma link C++ class dd4hep::Handle<TGeoTrd2>+;

#pragma link C++ class dd4hep::Trd2+;
#pragma link C++ class dd4hep::Solid_type<TGeoTrd2>+;
#pragma link C++ class dd4hep::Handle<TGeoTrd2>+;

#pragma link C++ class dd4hep::Sphere+;
#pragma link C++ class dd4hep::Solid_type<TGeoSphere>+;
#pragma link C++ class dd4hep::Handle<TGeoSphere>+;

#pragma link C++ class dd4hep::Paraboloid+;
#pragma link C++ class dd4hep::Solid_type<TGeoParaboloid>+;
#pragma link C++ class dd4hep::Handle<TGeoParaboloid>+;

#pragma link C++ class dd4hep::Hyperboloid+;
#pragma link C++ class dd4hep::Solid_type<TGeoHype>+;
#pragma link C++ class dd4hep::Handle<TGeoHype>+;

#pragma link C++ class dd4hep::PolyhedraRegular+;
#pragma link C++ class dd4hep::Solid_type<TGeoPgon>+;
#pragma link C++ class dd4hep::Handle<TGeoPgon>+;

#pragma link C++ class dd4hep::BooleanSolid+;
#pragma link C++ class dd4hep::Solid_type<TGeoCompositeShape>+;
#pragma link C++ class dd4hep::Handle<TGeoCompositeShape>+;

#pragma link C++ class dd4hep::SubtractionSolid+;
#pragma link C++ class dd4hep::UnionSolid+;
#pragma link C++ class dd4hep::IntersectionSolid+;

#pragma link C++ class dd4hep::PlacedVolumeProcessor+;
#pragma link C++ class dd4hep::PlacedVolumeScanner+;

#endif  // __CINT__
#endif  /* DD4HEP_DDCORE_ROOTDICTIONARY_H  */
