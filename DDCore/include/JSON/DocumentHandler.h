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
#ifndef DD4HEP_DDCORE_JSON_DOCUMENTHANDLER_H
#define DD4HEP_DDCORE_JSON_DOCUMENTHANDLER_H

/// Framework include files
#include "JSON/Elements.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace json {

    /// Class supporting to read and parse XML documents.
    /**
     *  Wrapper object around the document parser.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_JSON
     */
    class DocumentHandler {
    public:
      /// Default constructor
      DocumentHandler();
      /// Default destructor
      virtual ~DocumentHandler();
      /// Load XML file and parse it.
      virtual Document load(const std::string& fname) const;
      /// Parse a standalong XML string into a document.
      virtual Document parse(const char* doc_string, size_t length) const;
    };

  }       /* End namespace json                    */
}         /* End namespace dd4hep                  */
#endif    /* DD4HEP_DDCORE_JSON_DOCUMENTHANDLER_H  */
