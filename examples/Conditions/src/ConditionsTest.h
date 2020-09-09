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
#ifndef EXAMPLES_CONDITIONS_SRC_CONDITIONSTEST_H
#define EXAMPLES_CONDITIONS_SRC_CONDITIONSTEST_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionsMap.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/detail/ConditionsInterna.h"
#include "DD4hep/ConditionsProcessor.h"

#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsManager.h"


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for implementation details of the AIDA detector description toolkit
  namespace ConditionExamples {

    using namespace cond;

    struct TestEnv {
      Detector& description;
      DetElement detector;
      ConditionsManager manager;
      Handle<ConditionsDataLoader> loader;
      const IOVType* epoch;
      const IOVType* run;

      TestEnv(Detector& _description, const std::string& detector_name);
      /// Find daughter element of the detector object
      DetElement daughter(const std::string& sub_path)  const;

      void add_xml_data_source(const std::string& file, const std::string& iov);
      static void dump_detector_element(DetElement elt, ConditionsMap& map);
      static void dump_conditions_tree(DetElement elt, ConditionsMap& map);
    };

    template <typename T> void print_condition(Condition condition);
    template <typename T> void print_bound_value(Condition condition, const char* norm=0);
    template <typename T> void print_conditions(const RangeConditions& rc);
    void check_discrete_condition(Condition c, const IOV& iov);

    template<typename T> const T& access_val(Condition c)   {
      if ( !c->is_bound() )   {
        c.bind<T>();
      }
      return c.get<T>();
    }
  }
}
#endif // EXAMPLES_CONDITIONS_SRC_CONDITIONSTEST_H
