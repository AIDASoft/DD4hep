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
/* 
   Plugin invocation:
   ==================
   This plugin behaves like a main program.
   Invoke the plugin with something like this:

   geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_stress \
   -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml

   Populate the conditions store by hand for a set of IOVs.
   Then compute the corresponding alignment entries....

*/
// Framework include files
#include "ConditionExampleObjects.h"
#include "DD4hep/Factories.h"
#include "TStatistic.h"
#include "TTimeStamp.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::ConditionExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_ConditionExample_stress2
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int condition_example (Geometry::LCDD& lcdd, int argc, char** argv)  {
  string input;
  int    num_iov = 10;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-iovs",argv[i],4) )
      num_iov = ::atol(argv[++i]);
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_ConditionExample_stress2                 \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -iovs    <number>        Number of collision loads to be performed.      \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);
  installManagers(lcdd);

  /******************** Initialize the conditions manager *****************/
  ConditionsManager condMgr = ConditionsManager::from(lcdd);
  condMgr["PoolType"]       = "DD4hep_ConditionsMappedPool";
  condMgr["UserPoolType"]   = "DD4hep_ConditionsMapUserPool";
  condMgr["UpdatePoolType"] = "DD4hep_ConditionsLinearUpdatePool";
  condMgr.initialize();
  
  const IOVType*  iov_typ  = condMgr.registerIOVType(0,"run").second;
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }

  /******************** Now as usual: create the slice ********************/
  dd4hep_ptr<ConditionsSlice> slice(Conditions::createSlice(condMgr,*iov_typ));
  ConditionsKeys(DEBUG).process(lcdd.world(),0,true);
  ConditionsDependencyCreator(*slice,DEBUG).process(lcdd.world(),0,true);

  TStatistic cr_stat("Creation"), acc_stat("Access");
  // ++++++++++++++++++++++++ Now compute the conditions for each of these IOVs
  for(int i=0; i<num_iov; ++i)  {
    {
      TTimeStamp start;
      IOV iov(iov_typ, IOV::Key(1+i*10,(i+1)*10));
      ConditionsPool*   iov_pool = condMgr.registerIOV(*iov.iovType, iov.key());
      ConditionsCreator creator(condMgr, iov_pool, DEBUG);  // Use a generic creator
      creator.process(lcdd.world(),0,true);                 // Create conditions with all deltas
      TTimeStamp stop;
      cr_stat.Fill(stop.AsDouble()-start.AsDouble());
      printout(INFO,"Creating", "Setup %-6ld conditions for IOV:%-60s  [%8.3f sec]",
               creator.conditionCount, iov.str().c_str(),
               stop.AsDouble()-start.AsDouble());
    }   {
      TTimeStamp start;
      IOV req_iov(iov_typ,i*10+5);
      // Attach the proper set of conditions to the user pool
      ConditionsManager::Result res = condMgr.prepare(req_iov,*slice);
      TTimeStamp stop;
      acc_stat.Fill(stop.AsDouble()-start.AsDouble());
      // Now compute the tranformation matrices
      printout(INFO,"Compute","Total %-6ld conditions (S:%6ld,L:%6ld,C:%6ld,M:%4ld) of type %-25s [%8.3f sec]",
               res.total(), res.selected, res.loaded, res.computed, res.missing,
               req_iov.str().c_str(), stop.AsDouble()-start.AsDouble());
    }
  }
  printout(INFO,"Statistics","+=======================================================================");
  printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
           cr_stat.GetName(), cr_stat.GetMean(), cr_stat.GetMeanErr(), cr_stat.GetRMS(), cr_stat.GetN());
  printout(INFO,"Statistics","+  %-12s:  %11.5g +- %11.4g  RMS = %11.5g  N = %lld",
           acc_stat.GetName(), acc_stat.GetMean(), acc_stat.GetMeanErr(), acc_stat.GetRMS(), acc_stat.GetN());
  printout(INFO,"Statistics","+=======================================================================");
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_ConditionExample_stress2,condition_example)
