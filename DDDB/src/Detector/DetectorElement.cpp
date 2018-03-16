//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================

// Framework include files
#include "Detector/DetectorElement.h"
#include "DD4hep/Printout.h"


namespace gaudi  {
  typedef dd4hep::ConditionKey _K;
  const std::string        Keys::staticKeyName  ("DetElement-Info-Static");
  const Keys::itemkey_type Keys::staticKey = _K::itemCode(Keys::staticKeyName);
  const std::string        Keys::deKeyName     ("DetElement-Info-IOV");
  const Keys::itemkey_type Keys::deKey     = _K::itemCode(Keys::deKeyName);

  const std::string        Keys::alignmentsComputedKeyName("Alignments-Computed");
  const Keys::key_type     Keys::alignmentsComputedKey = _K::KeyMaker(0,Keys::alignmentsComputedKeyName).hash;
}

std::string gaudi::DE::indent(int level)   {
  char fmt[128], text[1024];
  ::snprintf(fmt,sizeof(fmt),"%03d %%-%ds",level+1,2*level+1);
  ::snprintf(text,sizeof(text),fmt,"");
  return text;
}

