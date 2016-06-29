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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DD4hep/Printout.h"
#include "DDDB/DDDBConversion.h"
#include "DDDB/DDDBConditionData.h"

using namespace DD4hep;

/// Default constructor
DDDB::DDDBConditionData::DDDBConditionData() : document(0) {
}

/// Default destructor
DDDB::DDDBConditionData::~DDDBConditionData()  {
  if ( document ) document->release();
  document = 0;
}

typedef DDDB::DDDBConditionData data_t;

/// print Conditions object
std::ostream& operator << (std::ostream& s, const data_t& data)   {
  struct _Print {
    void operator()(const data_t::Params::value_type& obj)  const {
      printout(INFO,"Condition","++ %-16s  %-8s -> %s",
	       obj.first.c_str(), 
	       typeName(obj.second.typeInfo()).c_str(), 
	       obj.second.str().c_str());
    }
  };
  if ( !data.params.empty() )  {
    for_each(data.params.begin(), data.params.end(),_Print());
  }
  return s;
}

/// Namespace for the AIDA detector description toolkit
namespace DD4hep  {
  /// Do NOTHING version! Function present to formally satisfy code. User implementation required
  int __eval_none(data_t*, const std::string&)  {
    return 1;
  }
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {
    int parse(data_t& data,const std::string& input) {
      if ( data.params.empty() || input.empty() )  {
	return 0;
      }
      return 1;
    }
  }
}

#include "DD4hep/ToStream.h"
#include "DD4hep/objects/BasicGrammar_inl.h"
#include "DD4hep/objects/ConditionsInterna.h"

DD4HEP_DEFINE_PARSER_GRAMMAR(data_t,__eval_none)
DD4HEP_DEFINE_CONDITIONS_TYPE(data_t)
