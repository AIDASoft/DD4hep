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
#ifndef DD4HEP_DDALIGN_ALIGNMENTREGISTER_H
#define DD4HEP_DDALIGN_ALIGNMENTREGISTER_H

// Framework includes
#include "DD4hep/Alignments.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {
    // Forward declarations
    class ConditionsSlice;
  }

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Alignments {
  
    // Forward declarations
    class AlignmentsUpdateCall;

    /// Create lignment dependencies from conditions
    /**
     *   Conditions analyser to select alignments and create the
     *   corresponding alignment condition dependencies.
     *
     *   This algorithm only serves as an example showing how 
     *   to program the alignments container of each detector
     *   element.
     *
     *   The procedure implemented here is as follows:
     *   For each registered and existing condition with a datatype
     *   Alignments::Delta and a flag Condition::ALIGNMENT,
     *   an entry is constructed to the alignments container.
     *   The entry is identified by a name returned by a call to
     *   construct_name(...). The optional alias is registered if the
     *   flag 'haveAlias' is set and the property 'alais' is not empty.
     *
     *   This does NOT mean, that every experiment must do it this way.
     *   The way to register the alignments with the detector element
     *   strongly depend on the surrounding experiment framework.
     *
     *   Note: We have to load one set of conditions in order to auto-populate
     *         because we need to see if a detector element actually has alignment
     *         conditions. For this we must access the conditions data.
     *         Unfortunate, but unavoidable.
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_DDALIGN
     */
    class AlignmentsRegister : public DetElement::Processor {
    public:
      /// Shortcut the ConditionsSlice type
      typedef Conditions::ConditionsSlice Slice;
    public:
      /// Reference to the alignment manager object
      Slice&                 slice;
      /// The callback to be registered for the update mechanism
      AlignmentsUpdateCall*  updateCall;
      /// Extension property to construct the name of the alignment condition
      std::string            extension;
      /// Name of the alignment alias for the detector elements alignment object
      std::string            alias;
      /// Flag if an alias to the real alignment object should be registered
      bool                   haveAlias;
      /// Print level
      PrintLevel             printLevel;
      
      /// Initializing constructor
      AlignmentsRegister(Slice& slice, AlignmentsUpdateCall* c);
      /// Default destructor
      virtual ~AlignmentsRegister();
      /// Callback to output conditions information
      virtual int processElement(DetElement de);
      /// Overloadable: call to construct the alignment conditions name.
      /**
       *   Specialize for user defined implementation.
       *
       *   Default implementation returns "cond.name()+extension".
       *   Please note, that the corrsponding implementation of
       *   'AlignmentsForward::construct_name' must match
       */
      virtual std::string construct_name(DetElement de, Conditions::Condition cond) const;
    };
  }       /* End namespace Alignments              */
}         /* End namespace DD4hep                  */
#endif    /* DD4HEP_DDALIGN_ALIGNMENTREGISTER_H    */
