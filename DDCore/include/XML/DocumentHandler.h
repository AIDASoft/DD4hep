// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_XML_DOCUMENTHANDLER_H
#define DD4hep_XML_DOCUMENTHANDLER_H

#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "XML/XMLElements.h"
#include <memory>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    class DocumentHandler {
    public:
      std::auto_ptr<xercesc::ErrorHandler> m_errHdlr;
      DocumentHandler();
      virtual ~DocumentHandler();
      virtual xercesc::XercesDOMParser* makeParser(xercesc::ErrorHandler* err_handler=0) const;
      virtual Document parse(const void* bytes, size_t length) const;
      virtual Document load(const std::string& fname) const;
    };
  }
}         /* End namespace DD4hep   */
#endif    /* DD4hep_XML_DOCUMENTHANDLER_H    */
