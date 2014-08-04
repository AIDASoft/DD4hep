// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DD4HEP_DD4HEPROOTPERSISTENCY_H
#define DD4HEP_DD4HEPROOTPERSISTENCY_H

// Framework include files
#include "DD4hep/LCDDData.h"

/// Helper class to support ROOT persistency of LCDD objects
class DD4hepRootPersistency : public TNamed, public DD4hep::Geometry::LCDDData  {
 public:
  /// Default constructor
 DD4hepRootPersistency() : TNamed() {}
  /// Default destructor
  virtual ~DD4hepRootPersistency() {}
  
  static int save(DD4hep::Geometry::LCDD& lcdd, const char* fname, const char* instance = "Geometry");
  static int load(DD4hep::Geometry::LCDD& lcdd, const char* fname, const char* instance = "Geometry");

  /// ROOT implementation macro
  ClassDef(DD4hepRootPersistency,1);
};

#endif    /* DD4HEP_DD4HEPROOTPERSISTENCY_H         */
