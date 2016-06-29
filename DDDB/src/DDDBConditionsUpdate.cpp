//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author  Markus Frank
//  \date    2016-02-02
//  \version 1.0
//
//==========================================================================
// $Id$

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DDCond/ConditionsInterna.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Plugin entry point.
static long update_dddb_conditions(lcdd_t& /* lcdd */, int argc, char** argv) {
  if ( argc > 0 )   {
    time_t evt_time = *(long*)argv[0];
    char   c_evt[64];
    struct tm evt;
    ::gmtime_r(&evt_time, &evt);
    ::strftime(c_evt,sizeof(c_evt),"%T %F",&evt);
    printout(INFO,"DDDB","+++ ConditionsUpdate: Updating DDDB conditions... event time:%s", c_evt);
    return 1;
  }
  except("DDDB","+++ Failed update DDDB conditions. No event time argument given!");
  return 0;
}
DECLARE_APPLY(DDDBConditionsUpdate,update_dddb_conditions)
