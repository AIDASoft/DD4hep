// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XML_CONFIG_H
#define DD4HEP_XML_CONFIG_H

#define  __TIXML__

#ifndef  __TIXML__

/* Setup XML parsing for the use of Apache Xerces-C
 *
 * @author M.Frank
 */
#include "xercesc/dom/DOMException.hpp"
XERCES_CPP_NAMESPACE_BEGIN
  class DOMElement;
  class DOMDocument;
  class DOMNodeList;
  class DOMNode;
  class DOMAttr;
  class DOMException;
XERCES_CPP_NAMESPACE_END
namespace DD4hep { namespace XML {  
    typedef XMLSize_t              XmlSize_t;
    typedef XMLCh                  XmlChar;
    typedef xercesc::DOMElement    XmlElement;
    typedef xercesc::DOMDocument   XmlDocument;
    typedef xercesc::DOMNodeList   XmlNodeList;
    typedef xercesc::DOMNode       XmlNode;
    typedef xercesc::DOMAttr       XmlAttr;
    typedef xercesc::DOMException  XmlException;
}}
#define XML_IMPLEMENTATION_TYPE " Apache Xerces-C DOM Parser"
#define XML_HEADER_DECLARATION

#else  // __TIXML__

#include <cstdlib>
/* Setup XML parsing for the use of TiXml
 *
 * @author M.Frank
 */
class TiXmlElement;
class TiXmlDocument;
class TiXmlNode;
class TiXmlAttribute;
namespace DD4hep { namespace XML {
    typedef std::size_t     XmlSize_t;
    typedef char            XmlChar;
    typedef TiXmlElement    XmlElement;
    typedef TiXmlDocument   XmlDocument;
    typedef void            XmlNodeList;
    typedef TiXmlNode       XmlNode;
    typedef TiXmlAttribute  XmlAttr;
}}
#define XML_IMPLEMENTATION_TYPE " TinyXML DOM mini-parser   "
#define XML_HEADER_DECLARATION  "<?xml version=\"1.0\" encoding=\"UTF-8\">\n"
#endif // __TIXML__

#endif // DD4HEP_XML_CONFIG_H
