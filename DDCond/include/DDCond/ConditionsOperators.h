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
#ifndef DDCOND_CONDITIONSOPERATORS_H
#define DDCOND_CONDITIONSOPERATORS_H

// Framework include files
#include "DDCond/ConditionsManager.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    // Forward declarations
    class ConditionsManagerObject;
    
    /// Helper class for common stuff used frequently
    class Operators  {
    public:
      /// Select all condition from the conditions manager registered at the Detector object
      static size_t collectAllConditions(Detector& description,  RangeConditions& conditions);
      /// Select all condition from the conditions manager registered at the Detector object
      static size_t collectAllConditions(ConditionsManager mgr, RangeConditions& conditions);
      /// Select all condition from the conditions manager registered at the Detector object
      static size_t collectAllConditions(Detector& description,  std::map<int,Condition>& conditions);
      /// Select all condition from the conditions manager registered at the Detector object
      static size_t collectAllConditions(ConditionsManager mgr, std::map<int,Condition>& conditions);
    };
  } /* End namespace cond             */
} /* End namespace dd4hep                   */

#endif // DDCOND_CONDITIONSOPERATORS_H
