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
#ifndef DDCORE_CONDITIONSDEBUG_H
#define DDCORE_CONDITIONSDEBUG_H

// Framework include files
#include "DD4hep/Conditions.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace cond {

    std::string cond_name(Condition c);
    std::string cond_name(const Condition::Object* c);
    
  } /* End namespace cond                 */
} /* End namespace dd4hep                 */
#endif     /* DDCORE_CONDITIONSDEBUG_H    */
