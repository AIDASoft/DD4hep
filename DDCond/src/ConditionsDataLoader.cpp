// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DD4hep/Handle.inl"
#include "DD4hep/Printout.h"
#include "DDCond/ConditionsInterna.h"
#include "DDCond/ConditionsDataLoader.h"

using std::string;
using namespace DD4hep::Conditions;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsDataLoader);

/// Default constructor
ConditionsDataLoader::ConditionsDataLoader(LCDD& lcdd, ConditionsManager mgr, const string nam) 
  : NamedObject(nam,"ConditionsDataLoader"), m_lcdd(lcdd), m_mgr(mgr)  {
  if ( m_mgr.isValid() ) return;
  except("ConditionsDataLoader","+++ Cannot create without a valid conditions manager handle!");
}

/// Default destructor
ConditionsDataLoader::~ConditionsDataLoader()   {
}

/// Add data source definition to loader
void ConditionsDataLoader::addSource(const string& source)   {
  m_sources.push_back(source);
}

/// Queue update to manager.
Condition ConditionsDataLoader::queueUpdate(Entry* data)   {
  return m_mgr->__queue_update(data);
}

/// Push update to manager.
void ConditionsDataLoader::pushUpdates()   {
  m_mgr->pushUpdates();
}
