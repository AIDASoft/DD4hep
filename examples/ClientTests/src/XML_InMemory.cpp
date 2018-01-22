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

 geoPluginRun -destroy -plugin DD4hep_XML-In-Memory -input <file name>

*/
// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DD4hep/DetectorLoad.h"
#include <fstream>
#include <cerrno>

using namespace std;
using namespace dd4hep;

/// Plugin function: Test in memory XML parsing of a simple sub detector
/**
 *  Factory: DD4hep_XML-In-Memory
 *
 *  Though there is a file name given, it is read FIRST and then parsed.
 *  Similar to a in memory XML string.
 *
 *  \author  M.Frank
 *  \version 1.0
 *  \date    20/01/2018
 */
static int XML_In_Memory (Detector& detector, int argc, char** argv)  {
  string input;
  bool arg_error = false;
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
      "     name:   factory name     DD4hep_AlignmentExample_read_xml                \n"
      "     -input   <string>        Geometry file                                   \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }
  ifstream is (input, ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
    char* buffer = new char[length+1];
    printout(INFO,"Read","Read a total of %ld charactors from %s.",
             length, input.c_str());
    // read data as a block:
    is.read (buffer,length);
    buffer[length] = 0;
    /// So here is the actual test: We parse the raw XML string
    DetectorLoad loader(detector);
    loader.processXMLString(buffer,0);
    return 1;
  }
  printout(INFO,"Read","FAILED to load xml data from %s [%s].",
           input.c_str(), ::strerror(errno));
  return 0;
}

DECLARE_APPLY(DD4hep_XML_InMemory,XML_In_Memory)
