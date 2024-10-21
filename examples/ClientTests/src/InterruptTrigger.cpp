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
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/SignalHandler.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <cstdio>
#include <csignal>
#include <unistd.h>

using namespace dd4hep;

static bool signal_handler(void* user_context, int signal)   {
  printout(ALWAYS,"SignalHandler",
	   "+++ Caught signal: %d context: %p. exit process.",
	   signal, user_context);
  ::fflush(stdout);
  ::exit(EINVAL);
  return true;
}

static Ref_t create_detector(Detector& , xml_h e, SensitiveDetector )  {
  // XML detector object: DDCore/XML/XMLDetector.h
  xml_dim_t x_det = e;  
  //Create the DetElement for dd4hep
  DetElement d_det(x_det.nameStr(),x_det.id());

  SignalHandler handler;
  handler.registerHandler(SIGINT, (void*)0xFEEDBABE, signal_handler);

  printout(ALWAYS,"SignalTrigger","+++ Sending interrupt signal to self...");
  ::kill(::getpid(), SIGINT);
  
  return d_det;
}

DECLARE_DETELEMENT(InterruptTrigger,create_detector)
