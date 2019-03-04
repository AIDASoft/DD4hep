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
#include "DD4hep/detail/OpticalSurfaceManagerInterna.h"
#include "DD4hep/detail/Handle.inl"

// C/C++ includes
#include <sstream>
#include <iomanip>

using namespace std;
using namespace dd4hep;

using detail::OpticalSurfaceManagerObject;
DD4HEP_INSTANTIATE_HANDLE(OpticalSurfaceManagerObject);

/// Default destructor
detail::OpticalSurfaceManagerObject::~OpticalSurfaceManagerObject() {
}
