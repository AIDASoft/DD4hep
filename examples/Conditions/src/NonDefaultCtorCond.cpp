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
#include "ConditionExampleObjects.h"

using namespace dd4hep::ConditionExamples;

NonDefaultCtorCond::NonDefaultCtorCond(int aa, int bb, int cc)
  : a(aa), b(bb), c(cc), d(0)
{
}

NonDefaultCtorCond::~NonDefaultCtorCond()   {
}

void NonDefaultCtorCond::set(int val)  {
  d = val;
}

#include "DD4hep/detail/Grammar_unparsed.h"
static auto s_registry = dd4hep::GrammarRegistry::pre_note<NonDefaultCtorCond>();
