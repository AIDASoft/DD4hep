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


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// Class supporting the interface of the LHCb conditions database to DD4hep
    /**
     *
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup DD4HEP_XML
     */
    class DDDBReader : public DD4hep::XML::UriReader   {
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
      void setMatch(const std::string& dir)      { m_match = dir;   }
      /// Access data match
      const std::string& match() const           { return m_match;  }
      /// Access to local context
      virtual UserContext* context()    {  return &m_context;  }
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, std::string& buffer);
      /// Resolve a given URI to a string containing the data
      virtual bool load(const std::string& system_id, UserContext* ctxt, std::string& buffer);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id);
      /// Inform reader about a locally (e.g. by XercesC) handled source load
      virtual void parserLoaded(const std::string& system_id, UserContext* ctxt);
      /// Read raw XML object from the database / file
      virtual int getObject(const std::string& system_id, UserContext* ctxt, std::string& data);

   protected:
      std::string       m_directory;
      std::string       m_match;
      DDDBReaderContext m_context;
    };
  }    /* End namespace DDDB            */
}      /* End namespace DD4hep          */
#endif /* DD4HEP_DDDB_DDDBFILEREADER_H  */
