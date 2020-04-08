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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionsData.h"

using namespace dd4hep::cond;

/// print Conditions object
std::ostream& operator << (std::ostream& s, const AbstractMap& data)   {
  struct _Print {
    void operator()(const AbstractMap::Params::value_type& obj)  const {
      if ( obj.second.typeInfo() == typeid(AbstractMap) )  {
        const AbstractMap& d= obj.second.get<AbstractMap>();
        dd4hep::printout(dd4hep::INFO,"Condition","++ %-16s [%d] %-8s -> %s",
                         obj.first.c_str(), d.classID,
                         obj.second.dataType().c_str(), 
                         obj.second.str().c_str());
      }
      else   {
        dd4hep::printout(dd4hep::INFO,"Condition","++ %-16s %-8s -> %s",
                         obj.first.c_str(),
                         obj.second.dataType().c_str(), 
                         obj.second.str().c_str());
      }
    }
  };
  if ( !data.params.empty() )  {
    for_each(data.params.begin(), data.params.end(), _Print());
  }
  return s;
}

/// Default destructor
ClientData::~ClientData()  {
}

/// Default constructor
AbstractMap::AbstractMap() : clientData(0), classID(0) {
  InstanceCount::increment(this);
}

/// Copy constructor
AbstractMap::AbstractMap(const AbstractMap& c)
  : clientData(c.clientData), params(c.params), classID(c.classID) 
{
  InstanceCount::increment(this);
}

/// Default destructor
AbstractMap::~AbstractMap()  {
  if ( clientData ) clientData->release();
  clientData = 0;
  InstanceCount::decrement(this);
}

/// Assignment operator
AbstractMap& AbstractMap::operator=(const AbstractMap& c)  {
  if ( this != &c )   {
    clientData = c.clientData;
    params = c.params;
    classID = c.classID;
  }
  return *this;
}

#include "DD4hep/detail/Grammar_unparsed.h"
static auto s_registry = dd4hep::GrammarRegistry::pre_note<AbstractMap>(1);
