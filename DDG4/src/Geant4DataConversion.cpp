// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4DataConversion.h"

using namespace DD4hep::Simulation;

/// Default constructor
Geant4ConversionHelper::Geant4ConversionHelper() {
}

/// Default destructor
Geant4ConversionHelper::~Geant4ConversionHelper() {
}

std::string Geant4ConversionHelper::encoding(Geometry::VolumeManager vm, Geometry::VolumeManager::VolumeID vid) {
  Geometry::PlacedVolume pv = vm.lookupPlacement(vid);
  Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
  Geometry::IDDescriptor id = sd.readout().idSpec();
  return id.fieldDescription();
}
