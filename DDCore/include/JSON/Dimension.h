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
#ifndef DD4HEP_JSON_DIMENSION_H
#define DD4HEP_JSON_DIMENSION_H

// Framework include files
#include "JSON/Elements.h"
#include "JSON/Tags.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace JSON {

    /// Re-use the defined interface for the XML data access
#include "XML/detail/Dimension.h"

  }       /* End namespace JSON       */
}         /* End namespace DD4hep     */
#endif    /* DD4HEP_JSON_DIMENSION_H  */
