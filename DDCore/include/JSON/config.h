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
#ifndef DD4HEP_DDCORE_JSON_CONFIG_H
#define DD4HEP_DDCORE_JSON_CONFIG_H

#include "Parsers/config.h"

#define DD4HEP_USE_BOOST_JSON 1

#include "boost/property_tree/ptree.hpp"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace json {

    typedef char                                    XmlChar;
    typedef boost::property_tree::ptree             ptree;
    typedef boost::property_tree::ptree             JsonDocument;
    typedef boost::property_tree::ptree::value_type JsonAttr;
    typedef boost::property_tree::ptree::value_type JsonElement;

  }       /* End namespace json              */
}         /* End namespace dd4hep            */
#endif    /* DD4HEP_DDCORE_JSON_CONFIG_H  */
