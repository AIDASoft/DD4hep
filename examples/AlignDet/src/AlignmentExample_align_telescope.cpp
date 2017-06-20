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
/* 
 Plugin invocation:
 ==================
 This plugin behaves like a main program.
 Invoke the plugin with something like this:

 A ideal detector may be worked on with:
 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_align_telescope  \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -setup file:${DD4hep_DIR}/examples/Conditions/data/manager.xml    \
              -end_plugin -print INFO

 To work an already loaded alignments use:

 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample_align_telescope  \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -setup file:${DD4hep_DIR}/examples/Conditions/data/repository.xml \
              -end_plugin -print INFO

*/
// Framework include files
#include "AlignmentExampleObjects.h"
#include "DDAlign/AlignmentsCalib.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::AlignmentExamples;
using align::AlignmentsCalib;

static void print_world_trafo(AlignmentsCalib& calib, const std::string& path)  {
  DetElement d(calib.detector(path));
  Alignment  a = calib.slice.get(d,align::Keys::alignmentKey);
  if ( a.isValid() )  {
    const double* tr = a.worldTransformation().GetTranslation();
    printout(INFO,"Example","++ World transformation of: %-32s  Tr:(%8.2g,%8.2g,%8.2g [cm])",
             path.c_str(), tr[0],tr[1],tr[2]);
    a.worldTransformation().Print();
    return;
  }
  Condition c = calib.slice.get(d,align::Keys::deltaKey);
  printout(WARNING,"Example",
           "++ Detector element:%s No alignment conditions present. Delta:%s",
           path.c_str(), c.isValid() ? "Present" : "Not availible");
}

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample_align_telescope
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int AlignmentExample_align_telescope (Detector& description, int argc, char** argv)  {
  string input, setup;
  bool arg_error = false, dump = false;

  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-setup",argv[i],5) )
      setup = argv[++i];
    else if ( 0 == ::strncmp("-dump",argv[i],5) )
      dump = true;
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || setup.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_AlignmentExample_align_telescope         \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -setup   <string>        Alignment setups (Conditions, etc)              \n"
      "     -dump                    Dump conditions user pool before and afterwards.\n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  description.fromXML(input);
  ConditionsManager manager = installManager(description);
  const void* setup_args[]  = {setup.c_str(), 0}; // Better zero-terminate

  description.apply("DD4hep_ConditionsXMLRepositoryParser",1,(char**)setup_args);
  // Now the deltas are stored in the conditions manager in the proper IOV pools
  const IOVType* iov_typ = manager.iovType("run");
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }
  IOV req_iov(iov_typ,1500);      // IOV goes from run 1000 ... 2000
  shared_ptr<ConditionsContent> content(new ConditionsContent());
  shared_ptr<ConditionsSlice>   slice(new ConditionsSlice(manager,content));
  ConditionsManager::Result cres = manager.prepare(req_iov,*slice);
  cond::fill_content(manager,*content,*iov_typ);

  // Collect all the delta conditions and make proper alignment conditions out of them
  map<DetElement, Delta> deltas;
  const auto coll = deltaCollector(*slice,deltas);
  auto proc = detectorProcessor(coll);
  //auto proc = detectorProcessor(deltaCollector(*slice,deltas));
  proc.process(description.world(),0,true);
  printout(INFO,"Prepare","Got a total of %ld deltas for processing alignments.",deltas.size());
  
  // ++++++++++++++++++++++++ Compute the tranformation matrices
  AlignmentsCalculator alignCalc;
  AlignmentsCalculator::Result ares = alignCalc.compute(deltas,*slice);
  printout(INFO,"Example",
           "Setup %ld/%ld conditions (S:%ld,L:%ld,C:%ld,M:%ld) (A:%ld,M:%ld) for IOV:%-12s",
           slice->conditions().size(),
           cres.total(), cres.selected, cres.loaded, cres.computed, cres.missing, 
           ares.computed, ares.missing, iov_typ->str().c_str());

  printout(INFO,"Example","=========================================================");
  printout(INFO,"Example","====  Alignment transformation BEFORE manipulation  =====");
  printout(INFO,"Example","=========================================================");
  if ( dump ) slice->pool->print("*");
  
  AlignmentsCalib calib(description,*slice);
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
  catch(const std::exception& e)  {
    printout(ERROR,"Example","Exception: %s.", e.what());
  }
  catch(...)  {
    printout(ERROR,"Example","UNKNOWN Exception....");
  }

  /// Let's change something:
  Delta delta(Position(333.0,0,0));
  calib.set(calib.detector("/world/Telescope"),Delta(Position(55.0,0,0)));
  calib.set(calib.detector("/world/Telescope/module_1"),delta);
  calib.set("/world/Telescope/module_3",delta);
  /// Commit transaction and push deltas to the alignment conditions
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

  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample_align_telescope,AlignmentExample_align_telescope)
