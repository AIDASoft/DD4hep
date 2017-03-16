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
#ifndef DD4HEP_XMLDETECTOR_H
#define DD4HEP_XMLDETECTOR_H

// Framework include files
#include "XML/XMLDimension.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class NamedObject;

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace XML {

    /// Include the implementation in the proper namespace
#include "XML/detail/Detector.h"

  }
}         /* End namespace DD4hep    */
#endif    /* DD4HEP_XMLDETECTOR_H    */
