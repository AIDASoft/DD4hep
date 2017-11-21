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
#ifndef DD4HEP_XML_CONFIG_H
#define DD4HEP_XML_CONFIG_H

#if      defined(DD4HEP_USE_TINYXML)
#define  __TIXML__
#endif

// C/C++ include files
#include <cstdlib>

/* Setup XML parsing for the use of Apache Xerces-C and TiXml
 *
 */

#define dd4hep Online

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
#else
    typedef unsigned short XmlChar;
#endif
  }
}

#ifdef  __TIXML__
#define XML_IMPLEMENTATION_TYPE " TinyXML DOM mini-parser   "
#else   // Xerces-C
#define XML_IMPLEMENTATION_TYPE " Apache Xerces-C DOM Parser"
#endif  // __TIXML__
#endif // DD4HEP_XML_CONFIG_H
