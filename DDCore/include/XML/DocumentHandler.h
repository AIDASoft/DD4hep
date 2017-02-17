//==========================================================================
//  AIDA Detector description implementation for LCD
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

#ifndef DD4HEP_XML_DOCUMENTHANDLER_H
#define DD4HEP_XML_DOCUMENTHANDLER_H

// Framework include files
#include "XML/XMLElements.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace XML {

    // Forward declarations
    class DocumentErrorHandler;
    class UriReader;

    /// Class supporting to read and parse XML documents.
    /**
     *  Wrapper object around the document parser.
     *  Supports both, XercesC and TiXml.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class DocumentHandler {
    public:
      /// Default constructor
      DocumentHandler();
      /// Default destructor
      virtual ~DocumentHandler();
      /// Default comment string
      static std::string defaultComment();
      // Create new XML document by parsing empty xml buffer
      Document create(const char* tag, const char* comment = 0) const;
      // Create new XML document by parsing empty xml buffer
      Document create(const std::string& tag, const std::string& comment) const;
      /// Load XML file and parse it.
      virtual Document load(const std::string& fname) const;
      /// Load XML file and parse it using URI resolver to read data.
      virtual Document load(const std::string& fname, UriReader* reader) const;
      /// Load secondary XML file with relative addressing with respect to handle
      virtual Document load(Handle_t base, const XmlChar* fname) const;
      /// Load secondary XML file with relative addressing with respect to handle
      virtual Document load(Handle_t base, const XmlChar* fname, UriReader* reader) const;
      /// Parse a standalong XML string into a document.
      virtual Document parse(const char* doc_string, size_t length) const;
      /// Parse a standalong XML string into a document using URI resolver to read data
      virtual Document parse(const char* doc_string, size_t length, const char* sys_id, UriReader* reader) const;
      /// Write xml document to output file (stdout if file name empty)
      virtual int output(Document doc, const std::string& fname) const;

      /// System ID of a given XML entity
      static std::string system_path(Handle_t base);
      /// System ID of a new XML entity in the same directory as base
      static std::string system_path(Handle_t base, const XmlChar* fname);
      /// System ID of a new XML entity in the same directory as base
      static std::string system_path(Handle_t base, const std::string& fname);
      /// System directory of a given XML entity
      static std::string system_directory(Handle_t base);
      /// System directory of a new XML entity in the same directory as base
      static std::string system_directory(Handle_t base, const XmlChar* fname);

    };
  }
} /* End namespace DD4hep            */
#endif    /* DD4HEP_XML_DOCUMENTHANDLER_H    */
