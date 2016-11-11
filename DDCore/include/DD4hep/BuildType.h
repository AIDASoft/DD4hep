//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-02-02
//  \version  1.0
//
//==========================================================================
#ifndef DD4HEP_BUILDTYPES_H 
#define DD4HEP_BUILDTYPES_H 1

// C/C++ include files
#include <string>


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Detector description build types.
  /** enum LCDDBuildType LCDD.h  DD4hep/LCDD.h
   * The corresponding flag is ONLY valid while parsing the
   * compact description. If no eometry, ie. at all other times
   * the accessor to the flag returns BUILD_NONE.
   */
  enum LCDDBuildType {
    BUILD_NONE = 0,
    BUILD_DEFAULT = 1,
    BUILD_SIMU = BUILD_DEFAULT,
    BUILD_RECO,
    BUILD_DISPLAY,
    BUILD_ENVELOPE
  };

  /// Translate string representation of the geometry build type to value
  LCDDBuildType buildType(const char* value);

  /// Translate string representation of the geometry build type to value
  LCDDBuildType buildType(const std::string& value);

} /* End namespace DD4hep             */
#endif  // DD4HEP_BUILDTYPES_H
