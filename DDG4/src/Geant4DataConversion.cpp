// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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

// Framework include files
#include "DDG4/Geant4DataConversion.h"

using namespace DD4hep::Simulation;

/// Default constructor
Geant4ConversionHelper::Geant4ConversionHelper() {
}

/// Default destructor
Geant4ConversionHelper::~Geant4ConversionHelper() {
}

/// Access to the data encoding using the volume manager and a specified volume id
std::string Geant4ConversionHelper::encoding(Geometry::VolumeManager vm, Geometry::VolumeManager::VolumeID vid) {
  Geometry::PlacedVolume pv = vm.lookupPlacement(vid);
  Geometry::SensitiveDetector sd = pv.volume().sensitiveDetector();
  return encoding(sd);
}

/// Access to the hit encoding in this sensitive detector
std::string Geant4ConversionHelper::encoding(Geometry::Handle<Geometry::SensitiveDetectorObject> sd)   {
  Geometry::IDDescriptor id = Geometry::SensitiveDetector(sd).readout().idSpec();
  return id.fieldDescription();
}

/// Access to the hit encoding in this readout object
std::string Geant4ConversionHelper::encoding(Geometry::Readout ro)   {
  Geometry::IDDescriptor id = ro.idSpec();
  return id.fieldDescription();
}
