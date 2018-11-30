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
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsDebug.h"
#include "DD4hep/detail/ConditionsInterna.h"

using std::string;
using namespace dd4hep::cond;

std::string dd4hep::cond::cond_name(const dd4hep::Condition::Object* c)  {
#if defined(DD4HEP_MINIMAL_CONDITIONS)
  dd4hep::ConditionKey::KeyMaker key(c->hash);
  char text[64];
  ::snprintf(text,sizeof(text),"%08X-%08X",key.values.det_key, key.values.item_key);
  return text;
#else
  return c->name;
#endif
}

string dd4hep::cond::cond_name(Condition c)    {
  return cond_name(c.ptr());
}
