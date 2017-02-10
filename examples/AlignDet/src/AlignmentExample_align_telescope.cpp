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

 A ideal detector may be worked on with:
 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_align_telescope  \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -setup file:${DD4hep_DIR}/examples/Conditions/data/manager.xml    \
              -direct -end_plugin -print INFO

 To work an already loaded alignments use:

 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_align_telescope  \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -setup file:${DD4hep_DIR}/examples/Conditions/data/repository.xml \
              -direct -end_plugin -print INFO

*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DDAlign/AlignmentsCalib.h"
#include "DDAlign/DDAlignUpdateCall.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::AlignmentExamples;

using Alignments::DDAlignUpdateCall;
using Alignments::AlignmentsCalib;
using Alignments::Delta;
using Geometry::Position;

static void print_world_trafo(AlignmentsCalib& calib, const std::string& path)  {
  DetAlign  d(calib.detector(path));
  Alignment a = d.alignments().get("Alignment",*calib.slice.pool);
  printout(INFO,"Example","++ World transformation of: %s", path.c_str());
  a->worldTransformation().Print();
}

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_align_telescope
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int AlignmentExample_align_telescope (Geometry::LCDD& lcdd, int argc, char** argv)  {
  string input, setup;
  bool arg_error = false, dump = false, direct = false;

  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-setups",argv[i],5) )
      setup = argv[++i];
    else if ( 0 == ::strncmp("-dump",argv[i],5) )
      dump = true;
    else if ( 0 == ::strncmp("-direct",argv[i],5) )
      direct = true;
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || setup.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_AlignmentExample_align_telescope         \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -setups  <string>        Alignment setups (Conditions, etc)              \n"
      "     -dump                    Dump conditions user pool before and afterwards.\n"
      "     -direct                  Perform realignment in DIRECT mode.             \n"
      "                              (no ConditionsManager callbacks)                \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);
  installManagers(lcdd);

  ConditionsManager condMgr  = ConditionsManager::from(lcdd);
  AlignmentsManager alignMgr = AlignmentsManager::from(lcdd);
  const void* setup_args[] = {setup.c_str(), 0}; // Better zero-terminate

  lcdd.apply("DD4hep_ConditionsXMLRepositoryParser",1,(char**)setup_args);
  // Now the deltas are stored in the conditions manager in the proper IOV pools
  const IOVType* iov_typ = condMgr.iovType("run");
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }
  IOV req_iov(iov_typ,1500);      // IOV goes from run 1000 ... 2000
  dd4hep_ptr<ConditionsSlice> slice(createSlice(condMgr,*iov_typ));
  ConditionsManager::Result   cres = condMgr.prepare(req_iov,*slice);

  // ++++++++++++++++++++++++ We need a valid set of conditions to do this!
  registerAlignmentCallbacks(lcdd,*slice);

  // ++++++++++++++++++++++++ Compute the tranformation matrices
  AlignmentsManager::Result ares = alignMgr.compute(*slice);
  printout(INFO,"Example",
           "Setup %ld/%ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
           slice->conditions().size(),
           cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, 
           ares.computed, ares.missing, iov_typ->str().c_str());

  printout(INFO,"Example","=========================================================");
  printout(INFO,"Example","====  Alignment transformation BEFORE manipulation  =====");
  printout(INFO,"Example","=========================================================");
  if ( dump ) slice->pool->print("*");
  
  
  AlignmentsCalib calib(lcdd,*slice);
  calib.derivationCall = new DDAlignUpdateCall();

  try  {  // These are only valid if alignments got pre-loaded!
    print_world_trafo(calib,"/world/Telescope");
    print_world_trafo(calib,"/world/Telescope/module_1");
    print_world_trafo(calib,"/world/Telescope/module_1/sensor");

    print_world_trafo(calib,"/world/Telescope/module_3");
    print_world_trafo(calib,"/world/Telescope/module_3/sensor");

    print_world_trafo(calib,"/world/Telescope/module_5");
    print_world_trafo(calib,"/world/Telescope/module_5/sensor");
    print_world_trafo(calib,"/world/Telescope/module_8/sensor");
  }
  catch(...)  {
  }
  
  // Alignments setup
  Alignment::key_type key_tel = calib.use("/world/Telescope");
  Alignment::key_type key_m1  = calib.use("/world/Telescope/module_1");
  Alignment::key_type key_m3  = calib.use("/world/Telescope/module_3");
  calib.start();  // Necessary to compute dependencies!

  /// Let's change something:
  Delta delta(Position(333.0,0,0));
  calib.setDelta(key_tel,Delta(Position(55.0,0,0)));
  calib.setDelta(key_m1,delta);
  calib.setDelta(key_m3,delta);

  if ( direct )
    calib.commit(AlignmentsCalib::DIRECT);
  else
    calib.commit();
  
  printout(INFO,"Example","=========================================================");
  printout(INFO,"Example","====  Alignment transformation AFTER manipulation   =====");
  printout(INFO,"Example","=========================================================");
  if ( dump ) slice->pool->print("*");

  print_world_trafo(calib,"/world/Telescope");
  print_world_trafo(calib,"/world/Telescope/module_1");
  print_world_trafo(calib,"/world/Telescope/module_1/sensor");
  print_world_trafo(calib,"/world/Telescope/module_2/sensor");

  print_world_trafo(calib,"/world/Telescope/module_3");
  print_world_trafo(calib,"/world/Telescope/module_3/sensor");
  print_world_trafo(calib,"/world/Telescope/module_4/sensor");

  print_world_trafo(calib,"/world/Telescope/module_5");
  print_world_trafo(calib,"/world/Telescope/module_5/sensor");
  print_world_trafo(calib,"/world/Telescope/module_6/sensor");
  print_world_trafo(calib,"/world/Telescope/module_7/sensor");
  print_world_trafo(calib,"/world/Telescope/module_8/sensor");

  
  //Alignments::AlignedVolumePrinter printer(slice->pool.get(),"Example");
  //Scanner().scan(printer,lcdd.world());

  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_align_telescope,AlignmentExample_align_telescope)
