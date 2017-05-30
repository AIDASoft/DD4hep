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
#ifndef DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H
#define DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H

// Framework includes
#include "DD4hep/ConditionsProcessor.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/Alignments.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace Alignments {

    /// Generic Alignments processor
    /**
     *   Please note that the principle of locality applies:
     *   The object is designed for stack allocation and configuration.
     *   It may NOT be shared across threads!
     *
     *   If applied to alignments container or detector elements,
     *   it is the user responsibility to beforehand set a valid
     *   alignments user pool containing the alignments registered
     *   to the detector element(s).  
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_ALIGNMENTS
     */
    class AlignmentsProcessor
      : public Alignment::Processor, public DetElement::Processor
    {
    protected:
      /// Reference to the user pool
      ConditionsMap* mapping;

    public:
      /// Initializing constructor
      AlignmentsProcessor(ConditionsMap* p) : mapping(p) {}
      /// Default destructor
      virtual ~AlignmentsProcessor() = default;
      /// Callback to output alignments information
      virtual int operator()(Alignment cond) override;
      /// Callback to output alignments information of an entire DetElement
      virtual int processElement(DetElement de) override;
    };

    /// Generic Alignment object collector
    /**
     *   Please see the documentation of the
     *   AlignmentsProcessor base class for further information.
     *
     */
    class AlignmentsCollector : virtual public AlignmentsProcessor  {
    public:
      /// Collection container
      std::vector<Alignment> alignments;
    public:
      /// Default constructor
      AlignmentsCollector(ConditionsMap* p) : AlignmentsProcessor(p) {}
      /// Default destructor
      virtual ~AlignmentsCollector() = default;
      /// Callback to output alignments information
      virtual int operator()(Alignment cond)  final  {
        alignments.push_back(cond);
        return 1;
      }
    };

    /// Generic Alignment-Delta collector keyed by detector elements
    /**
     *   To be used with utilities like DetElementProcessor etc.
     *
     */
    class DetElementDeltaCollector  {
    public:
      /// Reference to the user pool
      ConditionsMap* mapping;
      /// Collection container
      std::map<DetElement,Delta> deltas;
    public:
      /// Default constructor
      DetElementDeltaCollector(ConditionsMap* p) : mapping(p) {}
      /// Default destructor
      virtual ~DetElementDeltaCollector() = default;
      /// Callback to output alignments information
      virtual int operator()(DetElement de, int level)  final;
    };
    
    /// Helper to select all alignments, which belong to a single DetElement structure
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_ALIGNMENTS
     */
    class DetElementAlignmentsCollector : public Conditions::Condition::Processor  {
    public:
      Conditions::ConditionKey       key;
      mutable std::vector<Alignment> alignments;
    public:
      /// Standard constructor
      DetElementAlignmentsCollector(DetElement de) : key(de.key(),0)   {}
      /// Overloadable entry: Selection callback: return true if the alignment should be selected
      virtual int process(Conditions::Condition cond)  final;
    };
    
  }    /* End namespace Alignments  */
}      /* End namespace DD4hep      */
#endif /* DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H  */
