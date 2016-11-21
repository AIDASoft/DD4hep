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
#ifndef DD4HEP_ALIGMENTS_ALIGNMENTUPDATECALL_H
#define DD4HEP_ALIGMENTS_ALIGNMENTUPDATECALL_H

// Framework include files
#include "DD4hep/ConditionDerived.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/Alignments.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the alignment part of the AIDA detector description toolkit
  namespace Alignments {

    /// Specialized conditions update callback for alignment condition objects
    /**
     *  Used by clients to update an alignment condition.
     *  The input data, a condition with an AbstractMap data structure
     *  containing the alignment delta data.
     *
     *  The condition must be fully computed in two steps:
     *  - firstly it is created here and the alignment delta copied.
     *  - secondly, once all updates are processed, the world-transformations
     *    for the alignment object(s) are computed by the AlignmentManager
     *    objects.
     *  The AlignmentManager also keeps track of newly created alignment 
     *  conditions and is informed in the callback.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class AlignmentUpdateCall : public Conditions::ConditionUpdateCall  {
    public:
      typedef Conditions::Condition              Condition;
      typedef Conditions::ConditionKey           ConditionKey;
      typedef Conditions::ConditionUpdateContext UpdateContext;
      typedef Alignments::AlignmentData          Data;
      typedef Alignments::AlignmentData::Delta   Delta;

    public:
      /// Default constructor
      AlignmentUpdateCall();

      /// Default destructor
      virtual ~AlignmentUpdateCall();

      /// Interface to client Callback in order to update the condition. To be overloaded by sub-class
      virtual Condition operator()(const ConditionKey& key, const UpdateContext& context) = 0;

      /// Callback to build the alignment conditions object and assign the delta
      /** Please Note:
       *  We MUST register the undigested condition to the alignment manager in order to
       *  later be able to compute the derived transformations.
       *  The derived transfomrations can only be computed 'later' once ALL delta stubs
       *  of a given consisten set of alignment transactions are present. The trafo
       *  computation must take place in a second pass. 
       *  This is necessary, because the parent information may actually be supplied also 'later'.
       * 
       *  We also cannot do this is some 'generic' way, because the delta is provided outside
       *  and may very well depend on the extrnal database technology used to save conditions.
       */
      virtual Condition handle(const ConditionKey& key, const UpdateContext& context, const Delta& delta);

      /// Handler to be called if the Alignment cannot be created due to a bad underlying data type.
      virtual Condition invalidDataType(const ConditionKey& key, const UpdateContext& context);
    };
  } /* End namespace Aligments                        */
} /* End namespace DD4hep                             */
#endif    /* DD4HEP_ALIGMENTS_ALIGNMENTUPDATECALL_H   */
