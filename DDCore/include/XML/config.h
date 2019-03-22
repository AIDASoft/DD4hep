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
//
// Setup XML parsing for the use of Apache Xerces-C and TiXml
//
//==========================================================================
#ifndef DD4HEP_XML_CONFIG_H
#define DD4HEP_XML_CONFIG_H

#include "Parsers/config.h"

#if      defined(DD4HEP_USE_TINYXML)
#define  __TIXML__
#endif

// C/C++ include files
#include <cstdlib>
#include <cstdint>
#include <uchar.h>

#ifndef  __TIXML__
// This is the absolute minimal include necessary to comply with XercesC
// Not includuing this file leads to clashes in XmlChar aka XMLCh in XercesC.
//
// We do not load here many dependencies. This simply sets up primitive types.
#include <xercesc/util/Xerces_autoconf_config.hpp>
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace xml {
    class XmlElement;
    class XmlDocument;
    class XmlNodeList;
    class XmlNode;
    class XmlAttr;
    typedef std::size_t XmlSize_t;
#ifdef  __TIXML__
    typedef char XmlChar;
#elif defined(XERCES_XMLCH_T)
    /// Use the definition from the autoconf header of Xerces:
    typedef XERCES_XMLCH_T XmlChar;
#else
    // These only work for very specific XercesC implementations:
    typedef char16_t       XmlChar;
    //typedef unsigned short XmlChar;
#endif
  }
}

#ifdef  __TIXML__
#define XML_IMPLEMENTATION_TYPE " TinyXML DOM mini-parser   "
#else   // Xerces-C
#define XML_IMPLEMENTATION_TYPE " Apache Xerces-C DOM Parser"
#endif  // __TIXML__
#endif  // DD4HEP_XML_CONFIG_H
