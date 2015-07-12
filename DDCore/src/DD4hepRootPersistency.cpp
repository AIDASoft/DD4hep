// $Id$
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
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepRootPersistency.h"

// ROOT include files
#include "TFile.h"

ClassImp(DD4hepRootPersistency)

typedef DD4hep::Geometry::LCDD LCDD;
typedef DD4hep::Geometry::LCDDData LCDDData;

int DD4hepRootPersistency::save(DD4hep::Geometry::LCDD& lcdd, const char* fname, const char* instance)   {
  TFile* f = TFile::Open(fname,"RECREATE");
  if ( f && !f->IsZombie()) {
    DD4hepRootPersistency* persist = new DD4hepRootPersistency();
    persist->adoptData(dynamic_cast<LCDDData&>(lcdd));
    int nBytes = persist->Write(instance);
    f->Close();
    DD4hep::printout(DD4hep::ALWAYS,"DD4hepRootPersistency",
                     "+++ Wrote %d Bytes of geometry data '%s' to '%s'.",
                     nBytes, instance, fname);
    delete f;
    delete persist;
    return nBytes;
  }
  DD4hep::printout(DD4hep::ERROR,"DD4hepRootPersistency","+++ Cannot open file '%s'.",fname);
  return 0;
}

int DD4hepRootPersistency::load(DD4hep::Geometry::LCDD& lcdd, const char* fname, const char* instance)  {
  TFile* f = TFile::Open(fname);
  if ( f && !f->IsZombie()) {
    DD4hepRootPersistency* persist = (DD4hepRootPersistency*)f->Get(instance);
    if ( persist )   {
      LCDDData& data = dynamic_cast<LCDDData&>(lcdd);
      data.adoptData(*persist);
      persist->clearData();
      delete persist;
      return 1;
    }
    DD4hep::printout(DD4hep::ERROR,"DD4hepRootPersistency",
                     "+++ Cannot Cannot load instance '%s' from file '%s'.",
                     instance, fname);
    f->ls();
    delete f;
    return 0;
  }
  DD4hep::printout(DD4hep::ERROR,"DD4hepRootPersistency","+++ Cannot open file '%s'.",fname);
  return 0;
}
