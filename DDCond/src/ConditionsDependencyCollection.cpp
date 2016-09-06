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
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DDCond/ConditionsDependencyCollection.h"

using namespace DD4hep;
using namespace DD4hep::Conditions;

/// Default constructor
ConditionsDependencyCollection::ConditionsDependencyCollection()
{
}

/// Copy constructor
ConditionsDependencyCollection::ConditionsDependencyCollection(const ConditionsDependencyCollection& copy)
  : dependencies(copy.dependencies)
{
  for(Dependencies::iterator i=dependencies.begin(); i!=dependencies.end(); ++i)
    (*i).second->addRef(), (*i).second.flag = 0;
}

/// Default destructor
ConditionsDependencyCollection::~ConditionsDependencyCollection()   {
  releaseObjects(dependencies);
}

/// Clear the dependency container. Destroys the contained stuff
void ConditionsDependencyCollection::clear()   {
  releaseObjects(dependencies);
}

/// Mini-container interface: insert element by key
std::pair<ConditionsDependencyCollection::iterator,bool>
ConditionsDependencyCollection::insert(const Dependencies::value_type& entry)   {
  entry.second->addRef();
  return dependencies.insert(entry);
}

/// Insert new element by key
std::pair<ConditionsDependencyCollection::iterator,bool>
ConditionsDependencyCollection::insert(Dependency* dep)   {
  dep->addRef();
  return dependencies.insert(std::make_pair(dep->key(),Holder(dep)));
}
