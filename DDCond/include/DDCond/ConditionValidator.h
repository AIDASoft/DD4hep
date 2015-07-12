// $Id: $
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
#ifndef DD4HEP_GEOMETRY_CONDITIONVALIDATOR_H
#define DD4HEP_GEOMETRY_CONDITIONVALIDATOR_H

// Framework includes
#include "DDCond/Condition.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Conditions validation class.
    /** 
     *  \author   M.Frank
     *  \version  1.0
     *  \ingroup  DD4HEP_CONDITIONS
     */
    class ConditionValidator  {
    public:
  
      /// Standard constructor
      ConditionValidator();
      /// Validate detector subtree and return conditions out of date
      bool validate(DetElement subtree, std::vector<Condition>& tobe_updated);

    };

  } /* End namespace Geometry               */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITIONVALIDATOR_H    */
