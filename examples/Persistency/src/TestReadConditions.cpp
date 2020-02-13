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

   geoPluginRun -plugin DD4hep_PersistencyExample_read_cond \
                -output <file-name>

   Test the writing of a bunch of standard conditions objects
   to a native ROOT file.

*/
// Framework include files
#include "PersistencySetup.h"
#include "DD4hep/Factories.h"
#include "TFile.h"
#include "DD4hep/AlignmentData.h"

// Define parser and Grammar for Delta and AlignmentData objects
#include "Parsers/Parsers.h"
DD4HEP_PARSERS_DECL_FOR_SINGLE(Delta)
DD4HEP_PARSERS_DECL_FOR_SINGLE(AlignmentData)
#include "DD4hep/detail/Grammar.h"

using namespace std;
using namespace dd4hep;
using namespace PersistencyExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_PersistencyExample_read_cond
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int persistency_example (Detector& /* description */, int argc, char** argv)  {
  string input;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-input",argv[i],4) )
      input = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || input.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_PersistencyExample_read_cond             \n"
      "     -input  <string>         Geometry input file                             \n"
      "     -iovs    <number>        Number of parallel IOV slots for processing.    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  // instantiate the grammars we will need
  dd4hep::BasicGrammar::instance<int>();
  dd4hep::BasicGrammar::instance<long>();
  dd4hep::BasicGrammar::instance<double>();
  dd4hep::BasicGrammar::instance<float>();
  dd4hep::BasicGrammar::instance<std::string>();

  dd4hep::BasicGrammar::instance<Delta>();
  dd4hep::BasicGrammar::instance<AlignmentData>();
  
  dd4hep::BasicGrammar::instance<std::vector<int>>();
  dd4hep::BasicGrammar::instance<std::vector<long>>();
  dd4hep::BasicGrammar::instance<std::vector<double>>();
  dd4hep::BasicGrammar::instance<std::vector<float>>();
  dd4hep::BasicGrammar::instance<std::vector<std::string>>();
  dd4hep::BasicGrammar::instance<std::map<std::string, int>>();

  PersistencyIO io;
  TFile*f = TFile::Open(input.c_str());
  f->ls();
  std::vector<dd4hep::Condition>* p = (std::vector<dd4hep::Condition>*)f->Get("Conditions");
  if ( p )  {
    int result = 0;
    for( const auto& cond : *p )  {
      //printout(INFO,"Example","+++ Reading condition object: %s",cond.name());
      result += printCondition(cond);
    }
    printout(ALWAYS,"Example","+++ Read successfully %ld conditions. Result=%d",p->size(),result);
  }
  else   {
    printout(ERROR,"Example","+++ ERROR +++ Failed to read object 'Conditions' from %s",f->GetName());
  }
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_PersistencyExample_read_cond,persistency_example)
