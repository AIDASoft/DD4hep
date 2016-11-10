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
#ifndef DD4HEP_ALIGNMENTS_ALIGNMENTSKEYASSIGN_H
#define DD4HEP_ALIGNMENTS_ALIGNMENTSKEYASSIGN_H

// Framework include files
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignments part of the AIDA detector description toolkit
  namespace Alignments   {

    /// Helper class to assign alignments keys based on patterms
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class AlignmentsKeyAssign  {
    public:
      Geometry::DetElement detector;

      /// Default constructor
      AlignmentsKeyAssign(Geometry::DetElement det) : detector(det) {}
      /// Copy constructor
      AlignmentsKeyAssign(const AlignmentsKeyAssign& c) = default;
      /// Assignment operator
      AlignmentsKeyAssign& operator=(const AlignmentsKeyAssign& c) = default;

      /// Add a new key to the alignments access map
      const AlignmentsKeyAssign& addKey(const std::string& key_value) const;
      /// Add a new key to the alignments access map: Allow for alias if key_val != data_val
      const AlignmentsKeyAssign& addKey(const std::string& key_value, const std::string& data_value) const;
    };
  } /* End namespace Alignments             */
} /* End namespace DD4hep                   */
#endif    /* DD4HEP_ALIGNMENTS_ALIGNMENTSKEYASSIGN_H */
