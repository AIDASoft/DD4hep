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
#include <DD4hep/Printout.h>
#include <DD4hep/Factories.h>
#include <DDDigi/DigiFactories.h>

// C/C++ include files
#include <cerrno>
#include <cstdlib>
#include <sstream>

#include <DDDigi/DigiInputAction.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiInputAction)

#include <DDDigi/DigiROOTInput.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiROOTInput)

#include <DDDigi/DigiSynchronize.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiSynchronize)

#include <DDDigi/DigiActionSequence.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiActionSequence)
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiParallelActionSequence)
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiSequentialActionSequence)

//#include <DDDigi/DigiSubdetectorSequence.h>
// DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiSubdetectorSequence)

#include <DDDigi/DigiLockedAction.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiLockedAction)

//#include <DDDigi/DigiSignalProcessorSequence.h>
// DECLARE_DIGISIGNALPROCESSOR_NS(dd4hep::digi,DigiSignalProcessorSequence)

#include <DDDigi/DigiStoreDump.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiStoreDump)

#include <DDDigi/DigiHitAttenuatorExp.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiHitAttenuatorExp)

#include <DDDigi/DigiContainerCombine.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiContainerCombine)

#include <DDDigi/DigiSegmentSplitter.h>
DECLARE_DIGISEGMENTACTION_NS(dd4hep::digi,DigiSegmentAction)
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiSegmentSplitter)

#include <DDDigi/DigiMultiContainerProcessor.h>
DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiMultiContainerProcessor)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiContainerProcessor)

/// Basic entry point 
static long dummy(dd4hep::Detector&, int, char**) {
  return 0;
}
DECLARE_APPLY(DDDigi_dummy,dummy)
