// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XML_CONFIG_H
#define DD4HEP_XML_CONFIG_H


#if      defined(DD4HEP_USE_TINYXML)
#define  __TIXML__
#endif

#include <cstdlib>

/* Setup XML parsing for the use of Apache Xerces-C and TiXml
 *
 * @author M.Frank
 */
namespace DD4hep { namespace XML {
    class XmlElement;
    class XmlDocument;
    class XmlNodeList;
    class XmlNode;
    class XmlAttr;
    typedef std::size_t    XmlSize_t;
#ifdef  __TIXML__
    typedef char           XmlChar;
#else
    typedef unsigned short XmlChar;
#endif
}}

#ifdef  __TIXML__
#define XML_IMPLEMENTATION_TYPE " TinyXML DOM mini-parser   "
#else   // Xerces-C
#define XML_IMPLEMENTATION_TYPE " Apache Xerces-C DOM Parser"
#endif  // __TIXML__
#endif // DD4HEP_XML_CONFIG_H
