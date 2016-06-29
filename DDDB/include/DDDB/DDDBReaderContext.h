// $Id$
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
#ifndef DD4HEP_DDDB_DDDBREADERCONTEXT_H
#define DD4HEP_DDDB_DDDBREADERCONTEXT_H

// Framework includes
#include "XML/UriReader.h"


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace of the DDDB conversion stuff
  namespace DDDB  {

    /// CondDB reader context to support intervals of validity
    /**
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDDB
     */
    class DDDBReaderContext : public XML::UriReader::UserContext  {
    public:
      long long int event_time, valid_since, valid_until;
      std::string doc, channel;
      /// Standard constructor
      DDDBReaderContext() : event_time(0), valid_since(0), valid_until(0) {}
      /// Copy constructor
      DDDBReaderContext(const DDDBReaderContext& c) 
        : XML::UriReader::UserContext(c), 
	event_time(c.event_time),
	valid_since(c.valid_since),
	valid_until(c.valid_until),
	channel(c.channel) {}
    };
  }    /* End namespace DDDB              */
}      /* End namespace DD4hep            */
#endif /* DD4HEP_DDDB_DDDBREADERCONTEXT_H */
