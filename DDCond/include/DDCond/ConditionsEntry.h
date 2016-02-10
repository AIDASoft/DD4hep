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
#ifndef DDCOND_CONDITIONSENTRY_H
#define DDCOND_CONDITIONSENTRY_H

// Framework include files
#include "DD4hep/NamedObject.h"
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class Entry;

    /// The intermediate conditions data used to populate the DetElement conditions
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class Entry : public NamedObject  {
    public:
      /// Reference to the detector element
      DetElement detector;
      /// The actual conditions data
      std::string value;
      /// The validity string to be interpreted by the updating engine
      std::string validity;
      /// Hash value of the name for fast identification
      int hash;
      /// Default constructor
      Entry() {}
      /// Initializing constructor
      Entry(const DetElement& det, const std::string& nam, const std::string& typ, const std::string& valid, int hash);
      /// Copy constructor
      Entry(const Entry& c);
      /// Default destructor
      virtual ~Entry();
      /// Assignment operator
      Entry& operator=(const Entry& c);
    };


  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif     /* DDCOND_CONDITIONSENTRY_H    */
