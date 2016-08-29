// $Id$
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
#ifndef DDCOND_CONDITIONSTEST_H
#define DDCOND_CONDITIONSTEST_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/DetConditions.h"
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/objects/ConditionsInterna.h"

#include "DDCond/ConditionsAccess.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsIOVPool.h"
#include "DDCond/ConditionsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Conditions {

    /// Namespace for test environments in DDCond
    namespace Test  {

      using Geometry::LCDD;
      using Geometry::Position;
      using Geometry::DetElement;

      struct TestEnv {
        LCDD& lcdd;
        DetElement detector;
        ConditionsManager manager;
        Handle<ConditionsDataLoader> loader;
        const IOVType* epoch;
        const IOVType* run;

        TestEnv(LCDD& _lcdd, const std::string& detector_name);
        /// Find daughter element of the detector object
        DetElement daughter(const std::string& sub_path)  const;

        void add_xml_data_source(const std::string& file, const std::string& iov);
        void dump_conditions_pools();
        static void dump_detector_element(DetElement elt);
        static void dump_conditions_tree(DetElement elt);
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
}
#endif // DDCOND_CONDITIONSTEST_H
