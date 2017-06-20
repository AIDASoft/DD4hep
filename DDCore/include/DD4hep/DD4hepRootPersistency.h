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
#ifndef DD4HEP_DD4HEPROOTPERSISTENCY_H
#define DD4HEP_DD4HEPROOTPERSISTENCY_H

// Framework include files
#include "DD4hep/DetectorData.h"

/// Helper class to support ROOT persistency of Detector objects
class DD4hepRootPersistency : public TNamed, public dd4hep::DetectorData  {
public:
  /// Default constructor
  DD4hepRootPersistency() : TNamed() {}
  /// Default destructor
  virtual ~DD4hepRootPersistency() {}

  static int save(dd4hep::Detector& description, const char* fname, const char* instance = "Geometry");
  static int load(dd4hep::Detector& description, const char* fname, const char* instance = "Geometry");

  /// ROOT implementation macro
  ClassDef(DD4hepRootPersistency,1);
};

#endif    /* DD4HEP_DD4HEPROOTPERSISTENCY_H         */
