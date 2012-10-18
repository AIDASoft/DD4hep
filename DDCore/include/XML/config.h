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

#define __TIXML__

#ifndef  __TIXML__

/* Setup XML parsing for the use of XercesC
 *
 * @author M.Frank
 */
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOMException.hpp"
namespace DD4hep { namespace XML {  
    typedef XMLSize_t              XmlSize_t;
    typedef XMLCh                  XmlChar;
    typedef xercesc::XMLString     XmlString;  
    typedef xercesc::DOMElement    XmlElement;
    typedef xercesc::DOMDocument   XmlDocument;
    typedef xercesc::DOMNodeList   XmlNodeList;
    typedef xercesc::DOMNode       XmlNode;
    typedef xercesc::DOMAttr       XmlAttr;
    typedef xercesc::DOMException  XmlException;
}}

#else  // __TIXML__

/* Setup XML parsing for the use of TiXml
 *
 * @author M.Frank
 */
#include "XML/tinyxml.h"
//#error("using tixml")
namespace DD4hep { namespace XML {
    typedef std::size_t     XmlSize_t;
    typedef char            XmlChar;
    typedef TiXmlElement    XmlElement;
    typedef TiXmlDocument   XmlDocument;
    typedef void            XmlNodeList;
    typedef TiXmlNode       XmlNode;
    typedef TiXmlAttribute  XmlAttr;
}}
#endif // __TIXML__
#endif // DD4HEP_XML_CONFIG_H
