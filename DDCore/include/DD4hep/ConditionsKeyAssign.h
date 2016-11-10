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
#ifndef DD4HEP_CONDITIONS_CONDITIONSKEYASSIGN_H
#define DD4HEP_CONDITIONS_CONDITIONSKEYASSIGN_H

// Framework include files
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    /// Helper class to assign conditions keys based on patterms
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsKeyAssign  {
    public:
      Geometry::DetElement detector;

      /// Default constructor
      ConditionsKeyAssign(Geometry::DetElement det) : detector(det) {}
      /// Copy constructor
      ConditionsKeyAssign(const ConditionsKeyAssign& c) = default;
      /// Assignment operator
      ConditionsKeyAssign& operator=(const ConditionsKeyAssign& c) = default;

      /// Add a new key to the conditions access map
      const ConditionsKeyAssign& addKey(const std::string& key_value) const;
      /// Add a new key to the conditions access map: Allow for alias if key_val != data_val
      const ConditionsKeyAssign& addKey(const std::string& key_value, const std::string& data_value) const;
    };
  } /* End namespace Conditions             */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_CONDITIONS_CONDITIONSKEYASSIGN_H */
