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

    class DocumentHandler {
    protected:
      std::auto_ptr<DocumentErrorHandler> m_errHdlr;
    public:
      DocumentHandler();
      virtual ~DocumentHandler();
      virtual Document load(Handle_t base, const XmlChar* fname) const;
      virtual Document load(const std::string& fname) const;
    };
  }
}         /* End namespace DD4hep            */
#endif    /* DD4hep_XML_DOCUMENTHANDLER_H    */
