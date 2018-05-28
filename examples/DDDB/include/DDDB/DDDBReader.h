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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================
#ifndef DD4HEP_DDDB_DDDBFILEREADER_H
#define DD4HEP_DDDB_DDDBFILEREADER_H

// Framework includes
#include "XML/UriReader.h"
#include "DDDB/DDDBReaderContext.h"
#include "DD4hep/ComponentProperties.h"

/// C++ include files
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// Class supporting the interface of the LHCb conditions database to dd4hep
    /**
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_DDDB
     */
    class DDDBReader : public dd4hep::xml::UriReader,
                       public PropertyConfigurable
    {
    public:
      /// Standard constructor
      DDDBReader(const std::string& dir="");
      /// Default destructor
      virtual ~DDDBReader()  {}
      /// Set data directory
      void setDirectory(const std::string& dir)  { m_directory = dir;   }
      /// Access data directory
      const std::string& directory() const       { return m_directory;  }
      /// Set data match
      void setMatch(const std::string& dir)      { m_match = dir;       }
      /// Access data match
      const std::string& match() const           { return m_match;      }
      /// Access to local context
      virtual UserContext* context()   override;

      /// Process load request. Calls getObject at the end.            */
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, std::string& buffer)  override;
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, UserContext* ctxt, std::string& buffer)  override;
      /** If you have to modify intervals of validity, do it in here.
       *  Only this routine is called by EVERY parsing request
       */
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id)  override;
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id, UserContext* ctxt)  override;

      /** Main object reader routine                                   */
      /// Read raw XML object from the database / file
      virtual int getObject(const std::string& system_id, UserContext* ctxt, std::string& data) = 0;


      /** Helpers for selective parsing  */
      /// Add a blocked path entry
      virtual void blockPath(const std::string& path)  override;
      /// Check if a URI path is blocked
      virtual bool isBlocked(const std::string& path)  const  override;
      
    protected:
      /// File directory
      std::string       m_directory;
      /// URI match string to invoke entity resolution
      std::string       m_match;
      /// Blocked URI pathes
      std::set<std::string> m_blockedPathes;
      /// Reader context
      DDDBReaderContext m_context;
    };
    }    /* End namespace DDDB            */
  }      /* End namespace dd4hep          */
#endif /* DD4HEP_DDDB_DDDBFILEREADER_H  */
