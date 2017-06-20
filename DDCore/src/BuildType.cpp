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

// Framework includes
#include "DD4hep/BuildType.h"

// C/C++ include files
#include <stdexcept>
#include <cstring>
#include <string>

/// Translate string representation of the geometry build type to value
dd4hep::DetectorBuildType dd4hep::buildType(const char* value)   {
  if ( !value )
    return BUILD_DEFAULT;
  else if ( strncmp(value,"BUILD_DEFAULT",9)==0 )
    return BUILD_DEFAULT;
  else if ( strncmp(value,"BUILD_SIMU",9)==0 )
    return BUILD_SIMU;
  else if ( strcmp(value,"1")==0 )
    return BUILD_SIMU;
  else if ( strncmp(value,"BUILD_RECO",9)==0 )
    return BUILD_RECO;
  else if ( strcmp(value,"2")==0 )
    return BUILD_RECO;
  else if ( strncmp(value,"BUILD_DISPLAY",9)==0 )
    return BUILD_DISPLAY;
  else if ( strcmp(value,"3")==0 )
    return BUILD_DISPLAY;
  else if ( strncmp(value,"BUILD_ENVELOPE",9)==0 )
    return BUILD_ENVELOPE;
  else if ( strcmp(value,"4")==0 )
    return BUILD_ENVELOPE;
  throw std::runtime_error(std::string("Invalid build type value: ")+value);
}

/// Translate string representation of the geometry build type to value
dd4hep::DetectorBuildType dd4hep::buildType(const std::string& value)   {
  return buildType(value.c_str());
}
