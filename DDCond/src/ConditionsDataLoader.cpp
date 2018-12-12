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
#include "DDCond/ConditionsDataLoader.h"
#include "DDCond/ConditionsManagerObject.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/Printout.h"

using std::string;
using namespace dd4hep::cond;

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConditionsDataLoader);

/// Default constructor
ConditionsDataLoader::ConditionsDataLoader(Detector& description, ConditionsManager mgr, const string nam) 
  : NamedObject(nam,"ConditionsDataLoader"), m_detector(description), m_mgr(mgr)
{
  if ( m_mgr.isValid() ) return;
  except("ConditionsDataLoader","+++ Cannot create loader without a valid conditions manager handle!");
}

/// Default destructor
ConditionsDataLoader::~ConditionsDataLoader()   {
}

/// Access to properties
dd4hep::Property& ConditionsDataLoader::operator[](const std::string& property_name)  {
  return properties().property(property_name);
}

/// Access to properties (CONST)
const dd4hep::Property& ConditionsDataLoader::operator[](const std::string& property_name)  const   {
  return properties().property(property_name);
}

/// Add data source definition to loader
void ConditionsDataLoader::addSource(const string& source, const IOV& iov)   {
  m_sources.push_back(make_pair(source,iov));
}

/// Add data source definition to loader
void ConditionsDataLoader::addSource(const string& source)   {
  m_sources.push_back(make_pair(source,IOV(0,0)));
}

/// Queue update to manager.
//Condition ConditionsDataLoader::queueUpdate(Entry* data)   {
//  return m_mgr->__queue_update(data);
//}

/// Push update to manager.
void ConditionsDataLoader::pushUpdates()   {
  m_mgr->pushUpdates();
}
