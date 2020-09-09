//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DD4HEP_CONDITIONSPROCESSOR_H
#define DD4HEP_CONDITIONSPROCESSOR_H

// Framework include files
#include "DD4hep/DetElement.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsMap.h"

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the conditions part of the AIDA detector description toolkit
  namespace cond   {


    /// Generic condition processor facade for the Conditons::Processor object
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(Condition cond) const
     *  The object is automatically wrapped to a Condition::Processor object
     *  and allows to scan trees using e.g. DetElementProcessors etc.
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class ConditionsProcessor : public Condition::Processor  {
      /// Reference to the actual processor
      T& processor;
    public:
      /// Default constructor
      ConditionsProcessor() = delete;
      /// Initializing constructor
      ConditionsProcessor(T& p) : processor(p) {}
      /// This move constructor is disabled to void temporary references.
      ConditionsProcessor(T&& p) = delete;
      /// R-value copy from a temporary copy (Since processor is reference)
      ConditionsProcessor(ConditionsProcessor&& copy) = default;
      /// Copy constructor
      ConditionsProcessor(const ConditionsProcessor& copy) = default;
      /// Default destructor
      virtual ~ConditionsProcessor() = default;
      /// Assignment operator
      ConditionsProcessor& operator=(const ConditionsProcessor& copy) = default;
      /// Processing callback
      virtual int process(Condition condition)  const override  {
        return (processor)(condition);
      }
    };
    /// Creator utility function for ConditionsProcessor objects
    template <typename T> inline
    ConditionsProcessor<typename std::remove_reference<T>::type> conditionsProcessor(T&& obj)
    { return ConditionsProcessor<typename std::remove_reference<T>::type>(obj);   }

    /// Generic condition processor facade for the Conditons::Processor object
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(Condition cond) const
     *  The object is automatically wrapped to a Condition::Processor object
     *  and allows to scan trees using e.g. DetElementProcessors etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class ConditionsProcessorWrapper : public Condition::Processor  {
      std::unique_ptr<T> processor;
    public:
      /// Default constructor
      ConditionsProcessorWrapper() = default;
      /// Initializing constructor
      ConditionsProcessorWrapper(T* p) : processor(p) {}
      /// Copy constructor
      ConditionsProcessorWrapper(const ConditionsProcessorWrapper& copy) = delete;
      /// Default destructor
      virtual ~ConditionsProcessorWrapper() = default;
      /// Assignment operator
      ConditionsProcessorWrapper& operator=(const ConditionsProcessorWrapper& copy) = delete;
      /// Processing callback
      virtual int process(Condition c)  const override  {
        return (*(processor.get()))(c);
      }
    };
    /// Creator utility function for ConditionsProcessorWrapper objects
    template <typename T> inline ConditionsProcessorWrapper<T>* createProcessorWrapper(T* obj)  {
      return new ConditionsProcessorWrapper<T>(obj);
    }
    /// Creator utility function for ConditionsProcessorWrapper objects
    template <typename T> inline ConditionsProcessorWrapper<T> processorWrapper(T* obj)  {
      return ConditionsProcessorWrapper<T>(obj);
    }

    /// Generic condition collector keyed by detector elements
    /**
     *   To be used with utilities like DetElementProcessor etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class ConditionsCollector  {
    public:
      /// Reference to the user pool
      ConditionsMap& mapping;
      /// Collection container
      T&             conditions;
    public:
      /// Default constructor
      ConditionsCollector(ConditionsMap& m, T& d) : mapping(m), conditions(d) {}
      /// Default move constructor is disabled
      ConditionsCollector(ConditionsMap& m, T&& p) = delete;
      /// R-value copy from a temporary
      ConditionsCollector(ConditionsCollector&& copy) = default;
      /// Copy constructor
      ConditionsCollector(const ConditionsCollector& copy) = default;
      /// Default destructor
      ~ConditionsCollector() = default;
      /// Assignment operator
      ConditionsCollector& operator=(const ConditionsCollector& copy) = default;
      /// Callback to output conditions information
      /** Note: Valid implementations exist for the container types:
       *        std::list<Condition>
       *        std::vector<Condition>
       *        std::map<DetElement,Condition>
       *        std::multimap<DetElement,Condition>
       *        std::map<std::string,Condition>        key = DetElement.path()
       *        std::multimap<std::string,Condition>   key = DetElement.path()
       */
      virtual int operator()(DetElement de, int level=0)  const final;
    };
    /// Creator utility function for ConditionsCollector objects
    template <typename T> inline
    ConditionsCollector<typename std::remove_reference<T>::type> conditionsCollector(ConditionsMap& m, T&& conditions)
    {  return ConditionsCollector<typename std::remove_reference<T>::type>(m, conditions); }
    

  }       /* End namespace cond               */
}         /* End namespace dd4hep                   */
#endif // DD4HEP_CONDITIONSPROCESSOR_H
