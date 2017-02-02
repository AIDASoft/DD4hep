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
#ifndef DD4HEP_XML_URIREADER_H
#define DD4HEP_XML_URIREADER_H

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace XML {

    /// Class supporting to read data given a URI
    /**
     *  Interface to use entity resolvers for parsing layered XML documents
     *  Currently only supported by the XercesC interface. 
     *  No support for TinyXML possible.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class UriReader {
    public:
      struct UserContext {
        UserContext() {}
        UserContext(const UserContext&) {}
        virtual ~UserContext() {}
      };
    public:
      /// Default constructor
      UriReader()  {}
      /// Default destructor
      virtual ~UriReader();
      /// Access to local context
      virtual UserContext* context()  {  return 0;  }
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, std::string& data);
      /// Resolve a given URI to a string containing the data with context
      virtual bool load(const std::string& system_id, UserContext* context, std::string& data) = 0;
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id, UserContext* ctxt) = 0;
    };

    /// Class supporting to read data given a URI
    /**
     *  Wrapper to read XML URI using a virtual reader instance.
     *  This implementation allows to externally chain an argument 
     *  structure to the resolution function, which allows to set/retrieve
     *  further arguments such as e.g. conditions IOVs.
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class UriContextReader : public UriReader  {
    protected:
      /// Pointer to true reader object
      UriReader* m_reader;
      /// Pointer to user context
      UriReader::UserContext* m_context;
    public:
      /// Default initializing constructor
      UriContextReader(UriReader* reader, UriReader::UserContext* ctxt);
      /// Copy constructor
      UriContextReader(const UriContextReader& copy);
      /// Default destructor
      virtual ~UriContextReader();
      /// Access to local context
      virtual UserContext* context()  {  return m_context;  }
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, std::string& data);
      /// Resolve a given URI to a string containing the data with context
      virtual bool load(const std::string& system_id, UserContext* context, std::string& data);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id, UserContext* ctxt);
    };

  }       /* End namespace XML               */
}         /* End namespace DD4hep            */
#endif    /* DD4HEP_XML_URIREADER_H          */
