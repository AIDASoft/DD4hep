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
#ifndef DDDB_DDDBREADERCONTEXT_H
#define DDDB_DDDBREADERCONTEXT_H

// Framework includes
#include "XML/UriReader.h"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// CondDB reader context to support intervals of validity
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class DDDBReaderContext : public xml::UriReader::UserContext  {
    public:
      long event_time  = 0;
      long valid_since = 0;
      long valid_until = 0;
      std::string doc, channel, match;
      /// Standard constructor
      DDDBReaderContext() = default;
      /// Copy constructor
      DDDBReaderContext(const DDDBReaderContext& c) 
        : xml::UriReader::UserContext(c), 
          event_time(c.event_time),
          valid_since(c.valid_since),
          valid_until(c.valid_until),
          channel(c.channel) {}
      /// Default destructor
      virtual ~DDDBReaderContext() = default;
      /// Assignment operator
      DDDBReaderContext& operator=(const DDDBReaderContext& c) = default;
    };
  }    /* End namespace DDDB              */
}      /* End namespace dd4hep            */
#endif // DDDB_DDDBREADERCONTEXT_H
