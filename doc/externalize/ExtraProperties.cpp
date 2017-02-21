//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
//  Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
//  All rights reserved.
//
//  For the licensing terms see OnlineSys/LICENSE.
//
//--------------------------------------------------------------------------
//  ExtraProperties.cpp
//--------------------------------------------------------------------------
//
//  Package    : Dataflow
//
//  Author     : Markus Frank
//==========================================================================
// Note:
//
// This is an example file, which shows how to instantiate extera parsers,
// which are not part of DD4hep, but may be interesting for a
// client application.
//
//==========================================================================

/// Framework includes
#include "DD4hep/Parsers.h"
/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the AIDA detector for utilities using boost::spirit parsers
  namespace Parsers {
    PARSERS_DECL_FOR_SINGLE(unsigned int)
    PARSERS_DECL_FOR_SINGLE(unsigned long)
  }
}
#include "DD4hep/BasicGrammar_inl.h"
#include "DD4hep/ComponentProperties_inl.h"

// C/C++ include files
#include <map>

typedef std::map<std::string, std::vector<std::string> > map_t;
DD4HEP_DEFINE_PARSER_GRAMMAR(map_t,eval_obj)
namespace DD4hep {
  DD4HEP_DEFINE_PROPERTY_TYPE(map_t);
}

DD4HEP_DEFINE_PARSER_GRAMMAR(unsigned int,eval_item)
DD4HEP_DEFINE_PARSER_GRAMMAR(unsigned long,eval_item)
/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  DD4HEP_DEFINE_PROPERTY_TYPE(unsigned int);
  DD4HEP_DEFINE_PROPERTY_TYPE(unsigned long);
}
