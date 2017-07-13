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

   geoPluginRun -plugin DD4hep_PersistencyExample_write_cond \
                -output <file-name>

   Test the writing of a bunch of standard conditions objects
   to a native ROOT file.

*/
// Framework include files
#include "PersistencySetup.h"
#include "DD4hep/Factories.h"

using namespace std;
using namespace dd4hep;
using namespace PersistencyExamples;

/// Plugin function: Condition program example
/**
 *  Factory: DD4hep_PersistencyExample_write_cond
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    01/12/2016
 */
static int persistency_example (Detector& /* description */, int argc, char** argv)  {
  string output;
  bool   arg_error = false;
  for(int i=0; i<argc && argv[i]; ++i)  {
    if ( 0 == ::strncmp("-output",argv[i],4) )
      output = argv[++i];
    else
      arg_error = true;
  }
  if ( arg_error || output.empty() )   {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                             \n"
      "     name:   factory name     DD4hep_PersistencyExample_write_cond                \n"
      "     -output  <string>        Geometry output file                            \n"
      "     -iovs    <number>        Number of parallel IOV slots for processing.    \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  std::vector<Condition> conditions;

  //Condition char_data    = make_condition<char>   ("char_data", 'A');
  //Condition shrt_data    = make_condition<short>  ("short_data",11);
  Condition int_data     = make_condition<int>    ("int_data",  12);
  Condition long_data    = make_condition<long>   ("long_data", 13);
  Condition dble_data    = make_condition<double> ("dble_data", 14.222);
  Condition flt_data     = make_condition<float>  ("flt_data",  15.88);
  Condition str_data     = make_condition<string> ("str_data",  string("Hello"));
  Condition delta_data   = make_condition<Delta>  ("delta_data",Delta(Position(333.0,0,0)));
  Condition align_data   = make_condition<AlignmentData>  ("align_data",AlignmentData());
  Condition alignment    = AlignmentCondition("Test#alignment");
  
  Condition flt_vector   = make_condition<vector<float> >  ("float_vector",{0.,1.,2.,3.,4.,5.,6.,7.,8.,9.});
  Condition dbl_vector   = make_condition<vector<double> > ("double_vector",{0.,1.,2.,3.,4.,5.,6.,7.,8.,9.});
  //Condition char_vector  = make_condition<vector<char> > ("char_vector",{'a','b','c','d','e','f','g','h','i','j'});
  //Condition shrt_vector  = make_condition<vector<short> >("short_vector",{0,10,20,30,40,50,60,70,80,90});
  Condition int_vector   = make_condition<vector<int> >    ("int_vector",{0,10,20,30,40,50,60,70,80,90});
  Condition long_vector  = make_condition<vector<long> >  ("long_vector",{0,10,20,30,40,50,60,70,80,90});
  Condition str_vector   = make_condition<vector<string> > ("str_vector",{"Hello","World","!","Here","I","am","!"});
  Condition string_map   = make_condition<map<string,int> >("string_map",{{"A",10},{"B",20},{"C",30}});

  //conditions.push_back(char_data);
  //conditions.push_back(shrt_data);
  conditions.push_back(int_data);
  conditions.push_back(long_data);
  conditions.push_back(dble_data);
  conditions.push_back(flt_data);
  conditions.push_back(str_data);

  conditions.push_back(flt_vector);
  conditions.push_back(dbl_vector);
  //conditions.push_back(char_vector);
  conditions.push_back(int_vector);
  conditions.push_back(long_vector);
  conditions.push_back(str_vector);
  
  conditions.push_back(string_map);

  /// Alignment stuff
  conditions.push_back(delta_data);
  conditions.push_back(align_data);
  conditions.push_back(alignment);
  
  PersistencyIO io;
  printout(INFO,"Example","+++ Writing generic conditions vector to %s",output.c_str());
  int nbytes = io.write(output,"Conditions",conditions);
  printout(INFO,"Example","+++ Wrote %d bytes to file %s",nbytes,output.c_str());
  printout(INFO,"Example","+++ %s %ld conditions to file.",
           nbytes > 0 ? "PASSED Wrote" : "FAILED +++ Could not write",conditions.size());
  // All done.
  return 1;
}

// first argument is the type from the xml file
DECLARE_APPLY(DD4hep_PersistencyExample_write_cond,persistency_example)
