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
#include "DD4hep/World.h"
#include "DD4hep/detail/DetectorInterna.h"

/// Access the detector descrion tree
dd4hep::Detector& dd4hep::World::detectorDescription() const   {
  return *(access()->description);
}
