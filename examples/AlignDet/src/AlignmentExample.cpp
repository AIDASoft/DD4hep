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

 geoPluginRun -volmgr -destroy -plugin DD4hep_AlignmentExample \
              -input file:${DD4hep_DIR}/examples/AlignDet/compact/Telescope.xml \
              -delta file:${DD4hep_DIR}/examples/Conditions/data/repository.xml 

*/
// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Conditions.h"
#include "DD4hep/Alignments.h"
#include "DD4hep/DetectorProcessor.h"
#include "DD4hep/AlignedVolumePrinter.h"
#include "DD4hep/Factories.h"

#include "DDCond/ConditionsManager.h"
#include "DDAlign/AlignmentsManager.h"
#include "DDAlign/DDAlignUpdateCall.h"
#include "DDAlign/DDAlignForwardCall.h"
#include "DDAlign/AlignmentsRegister.h"
#include "DDAlign/AlignmentsForward.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;

namespace {
  struct Proc : public Geometry::DetectorProcessor  {
    DetElement::Processor* proc;
    virtual int operator()(DetElement de, int)
    { return proc->processElement(de);                     }
    template <typename Q> Proc& scan(Q& p, DetElement start)
    { proc = &p; this->process(start, 0, true); return *this; }
  };
}

/// Plugin function: Alignment program example
/**
 *  Factory: DD4hep_AlignmentExample
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/04/2016
 */
static int alignment_example (Geometry::LCDD& lcdd, int argc, char** argv)  {

  string input, delta;
  bool arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else if ( 0 == ::strncmp("-deltas",argv[i],5) )
      delta = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() || delta.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_AlignmentExample                         \n"
      "     -input   <string>        Geometry file                                   \n"
      "     -deltas  <string>        Alignment deltas (Conditions                    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // First we load the geometry
  lcdd.fromXML(input);
  // Now we instantiate the conditions manager
  lcdd.apply("DD4hep_ConditionsManagerInstaller",0,(char**)0);
  // Now we instantiate the alignments manager
  lcdd.apply("DD4hep_AlignmentsManagerInstaller",0,(char**)0);

  Conditions::ConditionsManager condMgr  = Conditions::ConditionsManager::from(lcdd);
  AlignmentsManager alignMgr = AlignmentsManager::from(lcdd);
  const void* delta_args[] = {delta.c_str(), 0}; // Better zero-terminate

  lcdd.apply("DD4hep_ConditionsXMLRepositoryParser",1,(char**)delta_args);
  // Now the deltas are stored in the conditions manager in the proper IOV pools
  const IOVType* iov_typ = condMgr.iovType("run");
  if ( 0 == iov_typ )  {
    except("ConditionsPrepare","++ Unknown IOV type supplied.");
  }
  IOV iov(iov_typ,1500);                  // IOV goes from run 1000 ... 2000
  dd4hep_ptr<Conditions::UserPool>  pool; // Working set up conditions for this IOV

  long num_updated = condMgr.prepare(iov, pool);
  printout(DEBUG,"Example","Updated %ld conditions of type %s.",num_updated, iov_typ->str().c_str());

  AlignmentsRegister reg(alignMgr,new DDAlignUpdateCall(),pool.get());
  AlignmentsForward  fwd(alignMgr,new DDAlignForwardCall(),pool.get());
  // Let's register the callbacks to compute dependent conditions/alignments
  Proc()
    .scan(reg,lcdd.world())
    .scan(fwd,lcdd.world());

  // ++++++++++++++++++++++++ Now the registration is finished.

  // ++++++++++++++++++++++++ The following has to be done for each new IOV....

  // Need to compute the conditions sources
  num_updated = condMgr.prepare(iov, pool);
  printout(DEBUG,"Example","Updated %ld conditions of type %s.",num_updated, iov_typ->str().c_str());
  
  // Let's compute the tranformation matrices
  alignMgr.compute(pool);

  // What else ? let's print the result
  AlignedVolumePrinter printer(pool.get(),"ExamplePrint");
  Proc().scan(printer,lcdd.world());
  
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_AlignmentExample,alignment_example)
