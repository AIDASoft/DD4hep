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
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiInputAction)

#include <DDDigi/DigiOutputAction.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiOutputAction)

#include <DDDigi/DigiROOTOutput.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiROOTOutput)

#include <DDDigi/DigiSynchronize.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSynchronize)

#include <DDDigi/DigiActionSequence.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiActionSequence)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiParallelActionSequence)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSequentialActionSequence)

//#include <DDDigi/DigiSubdetectorSequence.h>
// DECLARE_DIGIEVENTACTION_NS(dd4hep::digi,DigiSubdetectorSequence)

#include <DDDigi/DigiLockedAction.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiLockedAction)

//#include <DDDigi/DigiSignalProcessorSequence.h>
// DECLARE_DIGISIGNALPROCESSOR_NS(dd4hep::digi,DigiSignalProcessorSequence)

#include <DDDigi/DigiStoreDump.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiStoreDump)

#include <DDDigi/DigiAttenuator.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiAttenuator)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiAttenuatorSequence)

#include <DDDigi/DigiContainerCombine.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiContainerCombine)

#include <DDDigi/DigiContainerDrop.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiContainerDrop)

#include <DDDigi/DigiSegmentSplitter.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSegmentSplitter)

#include <DDDigi/DigiContainerProcessor.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiMultiContainerProcessor)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiContainerSequence)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiContainerProcessor)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiContainerSequenceAction)

#include <DDDigi/DigiDepositMonitor.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositTimeMonitor)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositEnergyMonitor)
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositPositionMonitor)

/// Basic entry point 
static long dummy(dd4hep::Detector&, int, char**) {
  return 0;
}
DECLARE_APPLY(DDDigi_dummy,dummy)
