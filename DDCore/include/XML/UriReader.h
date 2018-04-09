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
#ifndef DD4HEP_XML_URIREADER_H
#define DD4HEP_XML_URIREADER_H

// C/C++ include files
#include <string>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace xml {

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
      /// Base class of the user context type chained to the entity resolver
      /*  User overloaded extensions hole the information necessary 
       *  to perform the entity resolution
       *  
       *  \author   M.Frank
       *  \version  1.0
       *  \ingroup DD4HEP_XML
       */
      struct UserContext {
        UserContext() = default;
        UserContext(const UserContext&) = default;
        virtual ~UserContext() = default;
      };
    public:
      /// Default constructor
      UriReader()  = default;
      /// Default destructor
      virtual ~UriReader();
      /// Access to local context
      virtual UserContext* context() = 0;
      /** Helpers for selective parsing  */
      /// Add a blocked path entry
      virtual void blockPath(const std::string&  /* path */)  {}
      /// Check if a URI path is blocked
      virtual bool isBlocked(const std::string& /* path */)  const  { return false; }
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
      virtual UserContext* context()  override  {  return m_context;  }
      /** Helpers for selective parsing  */
      /// Add a blocked path entry
      virtual void blockPath(const std::string& path)  override;
      /// Check if a URI path is blocked
      virtual bool isBlocked(const std::string& path)  const  override;
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, std::string& data)  override;
      /// Resolve a given URI to a string containing the data with context
      virtual bool load(const std::string& system_id, UserContext* context, std::string& data)  override;
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id)  override;
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id, UserContext* ctxt)  override;
    };

  }       /* End namespace xml               */
}         /* End namespace dd4hep            */
#endif    /* DD4HEP_XML_URIREADER_H          */
