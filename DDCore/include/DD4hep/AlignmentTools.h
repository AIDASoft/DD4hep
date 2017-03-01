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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTTOOLS_H
#define DD4HEP_ALIGMENTS_ALIGNMENTTOOLS_H

// Framework include files
#include "DD4hep/Alignments.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    namespace AlignmentTools   {

      /// Copy alignment object from source object
      void copy(Alignment from, Alignment to);

      /// Compute the ideal/nominal to-world transformation from the detector element placement
      /**
       *  Note: Detector information of the alignment data is filled by the caller!
       */
      void computeIdeal(Alignment alignment);

      /// Compute the ideal/nominal to-world transformation from the detector element placement
      /**
       *  Note: Detector information of the alignment data is filled by the caller!
       */
      //void computeIdeal(Alignment alignment, const Alignment::NodeList& node_list);

      /// Compute the survey to-world transformation.
      /** Compute the survey to-world transformation from 
       *  the detector element placement with respect to 
       *  the survey constants
       *
       *  Note: Detector information of the alignment data is filled by the caller!
       */
      void computeSurvey(Alignment alignment);


    }

  } /* End namespace Aligments               */
} /* End namespace DD4hep                    */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTTOOLS_H   */
