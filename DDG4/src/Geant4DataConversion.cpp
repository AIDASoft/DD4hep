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

// Framework include files
#include "DDG4/Geant4DataConversion.h"

using namespace dd4hep::sim;

/// Default constructor
Geant4ConversionHelper::Geant4ConversionHelper() {
}

/// Default destructor
Geant4ConversionHelper::~Geant4ConversionHelper() {
}

/// Access to the data encoding using the volume manager and a specified volume id
std::string Geant4ConversionHelper::encoding(VolumeManager vm, VolumeID vid) {
  PlacedVolume      pv = vm.lookupPlacement(vid);
  SensitiveDetector sd = pv.volume().sensitiveDetector();
  return encoding(sd);
}

/// Access to the hit encoding in this sensitive detector
std::string Geant4ConversionHelper::encoding(Handle<SensitiveDetectorObject> sd)   {
  IDDescriptor id = SensitiveDetector(sd).readout().idSpec();
  return id.fieldDescription();
}

/// Access to the hit encoding in this readout object
std::string Geant4ConversionHelper::encoding(Readout ro)   {
  IDDescriptor id = ro.idSpec();
  return id.fieldDescription();
}
