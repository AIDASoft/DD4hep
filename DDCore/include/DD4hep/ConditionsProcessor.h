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
#ifndef DD4HEP_DDCORE_CONDITIONSPROCESSOR_H
#define DD4HEP_DDCORE_CONDITIONSPROCESSOR_H

// Framework includes
#include "DD4hep/Conditions.h"
#include "DD4hep/Detector.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace Conditions {

    /// Generic Conditions processor
    /**
     *   Please note that the principle of locality applies:
     *   The object is designed for stack allocation and configuration.
     *   It may NOT be shared across threads!
     *
     *   If applied to conditions container or detector elements,
     *   it is the user responsibility to beforehand set a valid
     *   conditions user pool containing the conditions registered
     *   to the detector element(s).  
     *
     *   \author  M.Frank
     *   \version 1.0
     *   \date    31/03/2016
     *   \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsProcessor :
      public Condition::Processor,
      public Container::Processor,
      public Geometry::DetElement::Processor
    {
    protected:
      /// Self type definition
      typedef ConditionsProcessor self_t;
      /// Make DetElement type local
      typedef Geometry::DetElement DetElement;
      /// Reference to the user pool
      UserPool* m_pool;
    public:
      /// Initializing constructor
      ConditionsProcessor(UserPool* p) : m_pool(p) {}
      /// Default destructor
      virtual ~ConditionsProcessor() = default;
      /// Set pool
      void setPool(UserPool* value)  { m_pool = value; }
      /// Callback to output conditions information
      virtual int operator()(Condition cond);
      /// Container callback for object processing
      virtual int operator()(Container container);
      /// Callback to output conditions information of an entire DetElement
      virtual int operator()(DetElement de);
    };

    /// Generic Condition object collector
    /**
     *   Please see the documentation of the
     *   ConditionsProcessor base class for further information.
     *
     */
    class ConditionsCollector : public ConditionsProcessor  {
    public:
      /// Collection container
      std::vector<Condition> conditions;
    public:
      /// Default constructor
      ConditionsCollector() = default;
      /// Default destructor
      virtual ~ConditionsCollector() = default;
      /// Callback to output conditions information
      virtual int operator()(Condition cond)    {
        conditions.push_back(cond);
        return 1;
      }
      /// Container callback for object processing
      virtual int operator()(Container container)
      {  return this->self_t::operator()(container);      }
      /// Callback to output conditions information of an entire DetElement
      virtual int operator()(DetElement detector)
      {  return this->self_t::operator()(detector);       }
    };
  }    /* End namespace Conditions  */
}      /* End namespace DD4hep      */
#endif /* DD4HEP_DDCORE_CONDITIONSPROCESSOR_H  */
