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
#ifndef DD4HEP_GEOMETRY_CONDITIONSPROCESSOR_H
#define DD4HEP_GEOMETRY_CONDITIONSPROCESSOR_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsMap.h"

// C/C++ include files

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace Conditions   {

    
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
      public Geometry::DetElement::Processor
    {
    public:
      /// Reference to the user pool
      ConditionsMap* mapping;
    public:
      /// Initializing constructor
      ConditionsProcessor(ConditionsMap* p) : mapping(p) {}
      /// Default destructor
      virtual ~ConditionsProcessor() = default;
      /// Callback to output conditions information
      virtual int process(Condition cond)  override;
      /// Callback to output conditions information of an entire DetElement
      virtual int processElement(DetElement de)  override;
    };

    /// Generic Condition object collector
    /**
     *   Please see the documentation of the
     *   ConditionsProcessor base class for further information.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class ConditionsCollector : virtual public ConditionsProcessor  {
    public:
      /// Collection container
      std::vector<Condition> conditions;
    public:
      /// Default constructor
      ConditionsCollector(ConditionsMap* p) : ConditionsProcessor(p) {}
      /// Default destructor
      virtual ~ConditionsCollector() = default;
      /// Callback to output conditions information
      virtual int process(Condition cond)   final  {
        conditions.push_back(cond);
        return 1;
      }
    };
    
    /// Helper to select all conditions, which belong to a single DetElement structure
    /**
     *   Please see the documentation of the
     *   ConditionsProcessor base class for further information.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_CONDITIONS
     */
    class DetElementConditionsCollector : public Condition::Processor  {
    public:
      ConditionKey                   key;
      mutable std::vector<Condition> conditions;
    public:
      /// Standard constructor
      DetElementConditionsCollector(DetElement de) : key(de.key(),0)   {}
      /// Overloadable entry: Selection callback: return true if the condition should be selected
      virtual int process(Condition cond)  final;
    };
    
  }       /* End namespace Conditions               */
}         /* End namespace DD4hep                   */
#endif    /* DD4HEP_GEOMETRY_CONDITIONSPROCESSOR_H  */
