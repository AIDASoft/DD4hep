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
#include "DD4hep/Factories.h"
#include "DDDigi/DigiFactories.h"

// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <sstream>

#include "DDDigi/DigiInputAction.h"
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiInputAction)

#include "DDDigi/DigiActionSequence.h"
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiActionSequence)

#include "DDDigi/DigiSynchronize.h"
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSynchronize)

#include "DDDigi/DigiLockedAction.h"
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiLockedAction)

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;



/// Basic entry point 
/**
 *  Factory: DD4hep_GeometryDisplay
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2014
 */
static long dummy(Detector&, int, char**) {
  return 0;
}
DECLARE_APPLY(DDDigi_dummy,dummy)
