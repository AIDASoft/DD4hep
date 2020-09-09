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
#ifndef EXAMPLES_PERSISTENCY_SRC_PERSISTENCYSETUP_H
#define EXAMPLES_PERSISTENCY_SRC_PERSISTENCYSETUP_H

#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/Grammar.h"
#include "DD4hep/AlignmentData.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for persistency examples
  namespace PersistencyExamples {

    template<typename T> Condition make_condition(const std::string& name, T val)   {
      Condition cond("Test#"+name, name);
      T& value   = cond.bind<T>();
      value      = val;
      cond->hash = ConditionKey(detail::hash32("TestCondition"),name).hash;
      return cond;
    }

    /// Print conditions object
    int printCondition(Condition cond);

    class PersistencyIO   {
    public:
      /// Default constructor
      PersistencyIO();
      /// Default destructor
      virtual ~PersistencyIO();
      /// Generic object write method
      int write(const std::string& fname, const std::string& title, const std::type_info& typ, const void* object);
      /// Object write method
      template <typename T> int write(const std::string& fname, const std::string& title, const T& object)
      {
        return write(fname, title, typeid(T), &object);
      }
      
    };
    
  }       /* End namespace persistencyexamples            */
}         /* End namespace dd4hep                         */

#endif // EXAMPLES_PERSISTENCY_SRC_PERSISTENCYSETUP_H
