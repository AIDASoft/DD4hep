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
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepRootPersistency.h"

// ROOT include files
#include "TFile.h"

ClassImp(DD4hepRootPersistency)

using namespace dd4hep;

int DD4hepRootPersistency::save(Detector& description, const char* fname, const char* instance)   {
  TFile* f = TFile::Open(fname,"RECREATE");
  if ( f && !f->IsZombie()) {
    DD4hepRootPersistency* persist = new DD4hepRootPersistency();
    persist->adoptData(dynamic_cast<DetectorData&>(description));
    int nBytes = persist->Write(instance);
    f->Close();
    printout(ALWAYS,"DD4hepRootPersistency",
                     "+++ Wrote %d Bytes of geometry data '%s' to '%s'.",
                     nBytes, instance, fname);
    if ( nBytes > 0 )  {
      printout(ALWAYS,"DD4hepRootPersistency",
                       "+++ Successfully saved geometry data to file.");
    }
    delete f;
    delete persist;
    return nBytes;
  }
  printout(ERROR,"DD4hepRootPersistency","+++ Cannot open file '%s'.",fname);
  return 0;
}

int DD4hepRootPersistency::load(Detector& description, const char* fname, const char* instance)  {
  TFile* f = TFile::Open(fname);
  if ( f && !f->IsZombie()) {
    DD4hepRootPersistency* persist = (DD4hepRootPersistency*)f->Get(instance);
    if ( persist )   {
      DetectorData& data = dynamic_cast<DetectorData&>(description);
      data.adoptData(*persist);
      persist->clearData();
      delete persist;
      return 1;
    }
    printout(ERROR,"DD4hepRootPersistency",
                     "+++ Cannot Cannot load instance '%s' from file '%s'.",
                     instance, fname);
    f->ls();
    delete f;
    return 0;
  }
  printout(ERROR,"DD4hepRootPersistency","+++ Cannot open file '%s'.",fname);
  return 0;
}

namespace {
  int printDetElement(DetElement d)  {
    const DetElement::Children& children = d.children();
    int count = 1;
    printout(INFO,"checkDetector","+++ %-40s Level:%3d Key:%08X VolID:%016llX",
             d.name(), d.level(), d.key(), d.volumeID());
    for( const auto& c : children )
      count += printDetElement(c.second);
    return count;
  }
  void checkDetector(DetElement d)   {
    printout(INFO,"checkDetector","+++ Checking Sub-Detector: %-40s Key:%08X VolID:%016llX",
             d.name(), d.key(), d.volumeID());
    int count = printDetElement(d);
  }
}

const DD4hepRootCheck& DD4hepRootCheck::checkDetectors()  const   {
  const auto& dets = object->detectors();
  for( const auto& d : dets )
    checkDetector(d.second);
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkMaterials()  const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkReadouts()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkFields()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkRegions()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkIdSpecs()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkSensitives()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkLimitSets()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkVolManager()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkDefines()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkProperties()   const   {
  return *this;
}

const DD4hepRootCheck& DD4hepRootCheck::checkAll()   const   {
  return *this;
}
