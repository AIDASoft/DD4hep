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
#include "DD4hep/Alignments.h"
#include "DD4hep/Detector.h"

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
    class AlignmentsProcessor :
      public Alignment::Processor,
      public Container::Processor,
      public Geometry::DetElement::Processor
    {
    protected:
      /// Self type definition
      typedef AlignmentsProcessor self_t;
      /// Make DetElement type local
      typedef Geometry::DetElement DetElement;
      /// Reference to the user pool
      UserPool* m_pool;
    public:
      /// Initializing constructor
      AlignmentsProcessor(UserPool* p) : m_pool(p) {}
      /// Default destructor
      virtual ~AlignmentsProcessor() = default;
      /// Set pool
      void setPool(UserPool* value)  { m_pool = value; }
      /// Callback to output alignments information
      virtual int operator()(Alignment cond);
      /// Container callback for object processing
      virtual int operator()(Container container);
      /// Callback to output alignments information of an entire DetElement
      virtual int operator()(DetElement de);
    };

    /// Generic Alignment object collector
    /**
     *   Please see the documentation of the
     *   AlignmentsProcessor base class for further information.
     *
     */
    class AlignmentsCollector : public AlignmentsProcessor  {
    public:
      /// Collection container
      std::vector<Alignment> alignments;
    public:
      /// Default constructor
      AlignmentsCollector() = default;
      /// Default destructor
      virtual ~AlignmentsCollector() = default;
      /// Callback to output alignments information
      virtual int operator()(Alignment cond)    {
        alignments.push_back(cond);
        return 1;
      }
      /// Container callback for object processing
      virtual int operator()(Container container)
      {  return this->self_t::operator()(container);      }
      /// Callback to output alignments information of an entire DetElement
      virtual int operator()(DetElement detector)
      {  return this->self_t::operator()(detector);       }
    };
    
  }    /* End namespace Alignments  */
}      /* End namespace DD4hep      */
#endif /* DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H  */
