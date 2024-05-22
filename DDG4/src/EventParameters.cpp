//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//
//====================================================================

/// Framework include files
#include <DDG4/EventParameters.h>

/// C/C++ include files
#include <map>
#include <string>

void dd4hep::sim::EventParameters::setRunNumber(int runNumber) {
  m_runNumber = runNumber;
}
void dd4hep::sim::EventParameters::setEventNumber(int eventNumber) {
  m_eventNumber = eventNumber;
}
