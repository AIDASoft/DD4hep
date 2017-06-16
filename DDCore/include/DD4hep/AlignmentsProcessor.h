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
#ifndef DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H
#define DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H

// Framework includes
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the AIDA detector description toolkit supporting XML utilities
  namespace align {
    
    /// Generic alignment processor facade for the Conditons::Processor object
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(Alignment cond) const
     *  The object is automatically wrapped to a Alignment::Processor object
     *  and allows to scan trees using e.g. DetElementProcessors etc.
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class AlignmentsProcessor : public Alignment::Processor  {
      T& processor;
    public:
      /// Default constructor
      AlignmentsProcessor() = default;
      /// Initializing constructor
      AlignmentsProcessor(T& p) : processor(p) {}
      /// Default move constructor is disabled
      AlignmentsProcessor(T&& p) = delete;
      /// R-value copy from a temporary (Since processor is reference)
      AlignmentsProcessor(AlignmentsProcessor&& copy) = default;
      /// Copy constructor
      AlignmentsProcessor(const AlignmentsProcessor& copy) = default;
      /// Default destructor
      virtual ~AlignmentsProcessor() = default;
      /// Assignment operator
      AlignmentsProcessor& operator=(const AlignmentsProcessor& copy) = default;
      /// Processing callback
      virtual int operator()(Alignment alignment)  const override  {
        return (processor)(alignment);
      }
    };
    /// Creator utility function for AlignmentsProcessor objects
    template <typename T> inline
    AlignmentsProcessor<typename std::remove_reference<T>::type> alignmentsProcessor(T&& obj)
    {  return AlignmentsProcessor<typename std::remove_reference<T>::type>(obj);    }

    /// Generic alignment processor facade for the Conditons::Processor object
    /**
     *  This wrapper converts any object, which has the signature
     *  int operator()(Alignment cond) const
     *  The object is automatically wrapped to a Alignment::Processor object
     *  and allows to scan trees using e.g. DetElementProcessors etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class AlignmentsProcessorWrapper : public Alignment::Processor  {
      std::unique_ptr<T> processor;
    public:
      /// Default constructor
      AlignmentsProcessorWrapper() = default;
      /// Initializing constructor
      AlignmentsProcessorWrapper(T* p) : processor(p) {}
      /// Copy constructor
      AlignmentsProcessorWrapper(const AlignmentsProcessorWrapper& copy) = default;
      /// Default destructor
      virtual ~AlignmentsProcessorWrapper() = default;
      /// Assignment operator
      AlignmentsProcessorWrapper& operator=(const AlignmentsProcessorWrapper& copy) = default;
      /// Processing callback
      virtual int operator()(Alignment c)  const override  {
        return (*(processor.get()))(c);
      }
    };
    /// Creator utility function for AlignmentsProcessorWrapper objects
    template <typename T> inline AlignmentsProcessorWrapper<T>* createProcessorWrapper(T* obj)  {
      return new AlignmentsProcessorWrapper<T>(obj);
    }
    /// Creator utility function for AlignmentsProcessorWrapper objects
    template <typename T> inline AlignmentsProcessorWrapper<T> processorWrapper(T* obj)  {
      return AlignmentsProcessorWrapper<T>(obj);
    }

    /// Generic Alignment-Delta collector keyed by detector elements
    /**
     *   To be used with utilities like DetElementProcessor etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class DeltaCollector  {
    public:
      /// Reference to the user pool
      ConditionsMap& mapping;
      /// Collection container
      T&             deltas;
    public:
      /// Default constructor
      DeltaCollector(ConditionsMap& m, T& d) : mapping(m), deltas(d) {}
      /// Default move constructor is disabled
      DeltaCollector(ConditionsMap& m, T&& p) = delete;
      /// R-value copy from a temporary
      DeltaCollector(DeltaCollector&& copy) = default;
      /// Copy constructor
      DeltaCollector(const DeltaCollector& copy) = default;
      /// Default destructor
      ~DeltaCollector() = default;
      /// Assignment operator
      DeltaCollector& operator=(const DeltaCollector& copy) = default;
      /// Callback to output alignments information
      /** Note: Valid implementations exist for the container types:
       *        std::list<Delta>
       *        std::vector<Delta>
       *        std::map<DetElement,Delta>
       *        std::multimap<DetElement,Delta>
       *        std::map<std::string,Delta>        key = DetElement.path()
       *        std::multimap<std::string,Delta>   key = DetElement.path()
       */
      virtual int operator()(DetElement de, int level=0)  const final;
    };

    /// Creator function for alignment collector objects
    template <typename T> inline
    DeltaCollector<typename std::remove_reference<T>::type> deltaCollector(ConditionsMap& m, T&& deltas)
    {    return DeltaCollector<typename std::remove_reference<T>::type>(m, deltas);    }

    /// Generic alignment collector keyed by detector elements
    /**
     *   To be used with utilities like DetElementProcessor etc.
     *
     *  
     *  \author  M.Frank
     *  \version 1.0
     *  \date    01/04/2016
     */
    template <typename T> class AlignmentsCollector  {
    public:
      /// Reference to the user pool
      ConditionsMap& mapping;
      /// Collection container
      T&             alignments;
    public:
      /// Default constructor
      AlignmentsCollector(ConditionsMap& m, T& d) : mapping(m), alignments(d) {}
      /// Default move constructor is disabled
      AlignmentsCollector(ConditionsMap& m, T&& p) = delete;
      /// Copy constructor
      AlignmentsCollector(const AlignmentsCollector& copy) = default;
      /// Default destructor
      ~AlignmentsCollector() = default;
      /// Assignment operator
      AlignmentsCollector& operator=(const AlignmentsCollector& copy) = default;
      /// Callback to output alignments information
      /** Note: Valid implementations exist for the container types:
       *        std::list<Alignment>
       *        std::vector<Alignment>
       *        std::map<DetElement,Alignment>
       *        std::multimap<DetElement,Alignment>
       *        std::map<std::string,Alignment>        key = DetElement.path()
       *        std::multimap<std::string,Alignment>   key = DetElement.path()
       */
      virtual int operator()(DetElement de, int level=0)  const final;
    };

    /// Creator function for alignment collector objects
    template <typename T> inline
    AlignmentsCollector<typename std::remove_reference<T>::type> alignmentsCollector(ConditionsMap& m, T&& alignments)
    { return AlignmentsCollector<typename std::remove_reference<T>::type>(m, alignments); }

  }    /* End namespace align  */
}      /* End namespace dd4hep      */
#endif /* DD4HEP_DDALIGN_ALIGNMENTSPROCESSOR_H  */
