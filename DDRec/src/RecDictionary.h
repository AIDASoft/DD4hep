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
#ifndef DDREC_SRC_RECDICTIONARY_H
#define DDREC_SRC_RECDICTIONARY_H

// Framework include files
#include "DDRec/Material.h"
#include "DDRec/ISurface.h"
#include "DDRec/DetectorData.h"
#include "DDRec/DetectorSurfaces.h"
#include "DDRec/MaterialManager.h"
#include "DDRec/MaterialScan.h"
#include "DDRec/CellIDPositionConverter.h"
#include "DDRec/Surface.h"
#include "DDRec/SurfaceManager.h"
#include "DDRec/Vector3D.h"
#include "DDRec/Vector2D.h"

namespace {
  class RecDictionary {};
}

// -------------------------------------------------------------------------
// Regular dd4hep dictionaries
// -------------------------------------------------------------------------
#if defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dd4hep;
#pragma link C++ namespace dd4hep::detail;
#pragma link C++ namespace dd4hep::rec;

using namespace dd4hep;
using namespace dd4hep::rec;

// DDRec/CellIDPositionConverter.h
#pragma link C++ class CellIDPositionConverter+;


// DDRec/DetectorData.h
#pragma link C++ class FixedPadSizeTPCStruct+;

#pragma link C++ class ZPlanarStruct+;
#pragma link C++ class ZPlanarStruct::LayerLayout+;

#pragma link C++ class ZDiskPetalsStruct+;
#pragma link C++ class ZDiskPetalsStruct::LayerLayout+;

#pragma link C++ class ConicalSupportStruct+;

#pragma link C++ class LayeredCalorimeterStruct+;
#pragma link C++ class LayeredCalorimeterStruct::Layer+;

#pragma link C++ class NeighbourSurfacesStruct+;

#pragma link C++ class StructExtension<FixedPadSizeTPCStruct>+;
#pragma link C++ class StructExtension<ZPlanarStruct>+;
#pragma link C++ class StructExtension<ZDiskPetalsStruct>+;
#pragma link C++ class StructExtension<ConicalSupportStruct>+;
#pragma link C++ class StructExtension<LayeredCalorimeterStruct>+;
#pragma link C++ class StructExtension<NeighbourSurfacesStruct>+;

// DDRec/DetectorSurfaces.h
#pragma link C++ class DetectorSurfaces+;

// DDRec/ISurface.h
#pragma link C++ class IMaterial+;
#pragma link C++ class ISurface+;
#pragma link C++ class ICylinder+;
#pragma link C++ class ICone+;
#pragma link C++ class SurfaceType+;

// DDRec/Material.h
#pragma link C++ class MaterialData+;
#pragma link C++ class MaterialManager+;
#pragma link C++ class MaterialScan+;
#pragma link C++ class VolSurfaceBase+;
#pragma link C++ class VolSurface+;
#pragma link C++ class VolSurfaceList+;
#pragma link C++ class std::list< VolSurface >+;
#pragma link C++ class VolPlaneImpl+;
#pragma link C++ class VolCylinderImpl+;
#pragma link C++ class VolConeImpl+;
//#pragma link C++ class VolSurfaceHandle+;
#pragma link C++ class Surface+;
#pragma link C++ class CylinderSurface+;
#pragma link C++ class ConeSurface+;
#pragma link C++ class SurfaceList+;
#pragma link C++ class std::list< ISurface* >+;

#pragma link C++ class Vector2D+;
#pragma link C++ class Vector3D+;
#pragma link C++ class SurfaceManager-;
#pragma link C++ class std::multimap< unsigned long, ISurface*>+;

#endif

#endif // DDREC_SRC_RECDICTIONARY_H
