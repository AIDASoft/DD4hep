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
#ifndef DD4HEP_JSON_CHILDVALUE_H
#define DD4HEP_JSON_CHILDVALUE_H

// Framework include files
#include "JSON/Dimension.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  class NamedObject;

  /// Namespace for the AIDA detector description toolkit supporting JSON utilities
  namespace JSON {

    /// Include the implementation in the proper namespace
#include "XML/detail/ChildValue.h"

  }       /* End namespace JSON         */
}         /* End namespace DD4hep       */
#endif    /* DD4HEP_JSON_CHILDVALUE_H   */
