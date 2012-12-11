// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XML_DOCUMENTHANDLER_H
#define DD4HEP_XML_DOCUMENTHANDLER_H

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

    // Forward declarations
    struct DocumentErrorHandler;

    /** @class DocumentHandler XMLDetector.h XML/XMLDetector.h
     * 
     *  Wrapper object around the document parser.
     *  Supports both, XercesC and TiXml.
     *  
     *  @author  M.Frank
     *  @version 1.0
     */
    class DocumentHandler {
    protected:
      /// Reference to the error handler object
      std::auto_ptr<DocumentErrorHandler> m_errHdlr;
    public:
      /// Default constructor
      DocumentHandler();
      /// Default destructor
      virtual ~DocumentHandler();
      /// Load XML file and parse it.
      virtual Document load(const std::string& fname) const;
      /// Load secondary XML file with relative addressing with respect to handle
      virtual Document load(Handle_t base, const XmlChar* fname) const;
      /// Parse a standalong XML string into a document.
      virtual Document parse(const char* doc_string, size_t length) const;
    };
  }
}         /* End namespace DD4hep            */
#endif    /* DD4HEP_XML_DOCUMENTHANDLER_H    */
