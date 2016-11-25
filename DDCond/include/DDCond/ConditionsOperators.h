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
#ifndef DD4HEP_CONDITIONS_CONDITIONSOPERATORS_H
#define DD4HEP_CONDITIONS_CONDITIONSOPERATORS_H

// Framework include files
#include "DDCond/ConditionsManager.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    // Forward declarations
    class ConditionsManagerObject;
    
    /// Helper class for common stuff used frequently
    class Operators  {
    public:
      /// Select all condition from the conditions manager registered at the LCDD object
      static size_t collectAllConditions(Geometry::LCDD& lcdd,  RangeConditions& conditions);
      /// Select all condition from the conditions manager registered at the LCDD object
      static size_t collectAllConditions(ConditionsManager mgr, RangeConditions& conditions);
      /// Select all condition from the conditions manager registered at the LCDD object
      static size_t collectAllConditions(Geometry::LCDD& lcdd,  std::map<int,Condition>& conditions);
      /// Select all condition from the conditions manager registered at the LCDD object
      static size_t collectAllConditions(ConditionsManager mgr, std::map<int,Condition>& conditions);
    };
  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */

#endif /* DD4HEP_CONDITIONS_CONDITIONSOPERATORS_H  */
