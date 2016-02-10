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

// Framework include files
#include "ConditionsTest.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Conditions;
using Geometry::LCDD;
using Geometry::Position;
using Geometry::DetElement;

namespace  {


  void print_tpc_discrete_conditions(Test::TestEnv& env, int epoch_min, int epoch_max, int run_min, int run_max)   {
    IOV iov_epoch(env.epoch), iov_run(env.run);
    iov_epoch.keyData.first  = epoch_min;
    iov_epoch.keyData.second = epoch_max;
    iov_run.keyData.first    = run_min;
    iov_run.keyData.second   = run_max;

    env.manager.prepare(iov_run);
    env.manager.enable(iov_run);
    env.manager.prepare(iov_epoch);
    env.manager.enable(iov_epoch);

    Condition cond = env.manager.get(env.detector,"AmbientTemperature",iov_epoch);
    Test::print_condition<void>(cond);
    cond = env.detector.condition("AmbientTemperature",iov_epoch);
    Test::check_discrete_condition(cond, iov_epoch);
    Test::print_condition<void>(cond);
    cond = env.detector.condition("ExternalPressure",iov_epoch);
    Test::check_discrete_condition(cond, iov_epoch);
    Test::print_condition<void>(cond);
    cond = env.detector.condition("SomeMultiParams",iov_epoch);
    Test::check_discrete_condition(cond, iov_epoch);
    Test::print_condition<void>(cond);
    printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printout(INFO,"Example","SUCCESS: +++ Conditions access OK for iov:%s!",iov_epoch.str().c_str());
    printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

    cond = env.detector.condition("alignment",iov_run);
    Test::check_discrete_condition(cond, iov_run);
    Test::print_condition<void>(cond);
    cond = env.detector.condition("TPC_A_align",iov_run);
    Test::check_discrete_condition(cond, iov_run);
    Test::print_condition<void>(cond);
    cond = env.daughter("TPC_SideA").condition("alignment",iov_run);
    Test::check_discrete_condition(cond, iov_run);
    Test::print_condition<void>(cond);
    printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printout(INFO,"Example","SUCCESS: +++ DISCRETE Conditions access OK for iov:%s!",iov_run.str().c_str());
    printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }

  void print_tpc_discrete_conditions(Test::TestEnv& env)   {
    print_tpc_discrete_conditions(env, 1396887257, 1396887257, 563543, 563543);
    print_tpc_discrete_conditions(env, 1396887257, 1396887257, 234567, 234567);
    print_tpc_discrete_conditions(env, 1396887257, 1396887257, 563543, 563543);
  }

  void print_tpc_range_conditions(Test::TestEnv& env, int run_min, int run_max)   {
    RangeConditions cond;
    IOV iov_run(env.run);
    iov_run.keyData.first    = run_min;
    iov_run.keyData.second   = run_max;
    try {
      cond = env.manager.getRange(env.detector,"TPC_A_align",iov_run);
      Test::print_conditions<void>(cond);
      printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      printout(INFO,"Example","SUCCESS: +++ RANGE Conditions access OK for iov:%s!",iov_run.str().c_str());
      printout(INFO,"Example","SUCCESS: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    }
    catch(const std::exception& e)   {
      printout(INFO,"Example","FAILED:  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
      printout(INFO,"Example","FAILED:  +++ RANGE Conditions access FAILED for iov:%s!",iov_run.str().c_str());
      printout(INFO,"Example","FAILED:  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    }
  }

  int example1(LCDD& lcdd, int, char** )  {
    printout(INFO,"Example1","+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printout(INFO,"Example1","+++ Executing Conditions example No. 1: Test conditions access");
    printout(INFO,"Example1","+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Test::TestEnv env(lcdd, "TPC");

    env.add_xml_data_source("/examples/Conditions/xml/TPC.xml");
    env.add_xml_data_source("/examples/Conditions/xml/TPC_run_563543.xml");
    env.add_xml_data_source("/examples/Conditions/xml/TPC_run_234567.xml");
    print_tpc_discrete_conditions(env);
    env.dump_conditions_pools();

    print_tpc_range_conditions(env,234567,563543);  // Should fail !
    print_tpc_range_conditions(env,123456,563543);  // Should fail !

    env.add_xml_data_source("/examples/Conditions/xml/TPC_run_filler.xml");
    env.add_xml_data_source("/examples/Conditions/xml/TPC_run_123456.xml");
    print_tpc_range_conditions(env,123456,900000);  // Now it should succeed
    return 1;
  }

  int example2(LCDD& lcdd, int argc, char** argv)  {
    printout(INFO,"Example2","+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printout(INFO,"Example2","+++ Executing Conditions example No. 2: Dump conditions tree");
    printout(INFO,"Example2","+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    DetElement det = lcdd.world();
    string args = "";
    for(int i=0; i<argc; ++i) { args += argv[i], args += " "; };
    printout(INFO,"Example2","Args: %s",args.c_str());
    Test::TestEnv::dump_conditions_tree(det);
    return 1;
  }

  struct Callee  {
    int m_param;
    Callee() : m_param(0) {}
    void call(unsigned long tags, DetElement& det, void* param)    {
      if ( (tags&DetElement::CONDITIONS_CHANGED) )
        printout(INFO,"Callee","+++ Conditions update %s param:%p",det.path().c_str(),param);
      if ( (tags&DetElement::PLACEMENT_CHANGED) )  
        printout(INFO,"Callee","+++ Alignment update %s param:%p",det.path().c_str(),param);
    }
  };

  /// Callback conditions tree of a detector element
  void callback_install(DetElement elt, Callee* c) {
    Position local, global;
    const DetElement::Children& children = elt.children();
    elt.localToWorld(local, global);
    elt.callAtUpdate(DetElement::CONDITIONS_CHANGED|DetElement::PLACEMENT_CHANGED,c,&Callee::call);
    for(DetElement::Children::const_iterator j=children.begin(); j!=children.end(); ++j)
      callback_install((*j).second,c);
  }

  int DD4hep_CallbackInstallTest(LCDD& lcdd, int argc, char** argv)  {
    printout(INFO,"Example3","+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printout(INFO,"Example3","+++ Executing DD4hepCallbackInstallTest: Install user callbacks");
    printout(INFO,"Example3","+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    string args = "";
    for(int i=0; i<argc; ++i) { args += argv[i], args += " "; };
    printout(INFO,"Example3","Args: %s",args.c_str());
    DetElement det = lcdd.world();
    callback_install(det, new Callee());
    return 1;
  }
}


DECLARE_APPLY(DD4hepConditionsAccessTest,example1)
DECLARE_APPLY(DD4hepConditionsTreeDump,example2)
DECLARE_APPLY(DD4hepCallbackInstallTest,DD4hep_CallbackInstallTest)
