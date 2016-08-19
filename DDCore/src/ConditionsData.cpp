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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/ConditionsData.h"
#include <sstream>

using namespace DD4hep::Conditions;

/// print Conditions object
std::ostream& operator << (std::ostream& s, const AlignmentDelta& data)   {
  std::string res;
  std::stringstream str;
  str << "[" << data.position << "," << data.rotation << "," << data.pivot << "]";
  res = str.str();
  for(size_t i=0; i<res.length(); ++i)
    if ( res[i]=='\n' ) res[i] = ' ';
  return s << res;
}

/// Default constructor
AlignmentDelta::AlignmentDelta()   {
  InstanceCount::increment(this);
}

/// Copy constructor
AlignmentDelta::AlignmentDelta(const AlignmentDelta& c)
  : pivot(c.pivot), position(c.position), rotation(c.rotation)  
{
  InstanceCount::increment(this);
}

/// Default destructor
AlignmentDelta::~AlignmentDelta()   {
  InstanceCount::decrement(this);
}

/// Assignment operator
AlignmentDelta& AlignmentDelta::operator=(const AlignmentDelta& c)   {
  if ( &c != this )  {
    pivot = c.pivot;
    position = c.position;
    rotation = c.rotation;
  }
  return *this;
}

/// print Conditions object
std::ostream& operator << (std::ostream& s, const AbstractMap& data)   {
  struct _Print {
    void operator()(const AbstractMap::Params::value_type& obj)  const {
      const AbstractMap& d= obj.second.get<AbstractMap>();
      DD4hep::printout(DD4hep::INFO,"Condition","++ %-16s [%d] %-8s -> %s",
		       obj.first.c_str(), d.classID,
		       obj.second.dataType().c_str(), 
		       obj.second.str().c_str());
    }
  };
  if ( !data.params.empty() )  {
    for_each(data.params.begin(), data.params.end(),_Print());
  }
  return s;
}

/// Default destructor
ClientData::~ClientData()  {
}

/// Copy constructor
AbstractMap::AbstractMap(const AbstractMap& c)
  : clientData(c.clientData), params(c.params), classID(c.classID) 
{
  InstanceCount::increment(this);
}

/// Default constructor
AbstractMap::AbstractMap() : clientData(0) {
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

#include "DD4hep/ToStream.h"
#include "DD4hep/objects/ConditionsInterna.h"
DD4HEP_DEFINE_CONDITIONS_TYPE_DUMMY(AbstractMap)
DD4HEP_DEFINE_CONDITIONS_TYPE_DUMMY(AlignmentDelta)

#include "DD4hep/objects/BasicGrammar_inl.h"
DD4HEP_DEFINE_PARSER_GRAMMAR(AbstractMap,eval_none<AbstractMap>)
DD4HEP_DEFINE_PARSER_GRAMMAR(AlignmentDelta,eval_none<AlignmentDelta>)
