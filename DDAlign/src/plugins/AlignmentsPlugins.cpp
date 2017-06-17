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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::align;

// ======================================================================================
#include "DDAlign/GlobalAlignmentWriter.h"
long create_global_alignment_xml_file(Detector& description, int argc, char** argv)   {
  DetElement top;
  string output, path = "/world";
  bool enable_transactions = false, arg_error = false;
  for(int i=1; i<argc;++i) {
    if ( argv[i] && (argv[i][0]=='-' || argv[i][0]=='/') ) {
      const char* p = ::strchr(argv[i],'=');
      if ( p && strncmp(argv[i]+1,"-output",7)==0 )
        output = p+1;
      else if ( p && strncmp(argv[i]+1,"-path",5)==0 )
        path = p+1;
      else if ( strncmp(argv[i]+1,"-transactions",5)==0 )
        enable_transactions = true;
      else
        arg_error = true;
    }
  }

  if ( arg_error || output.empty() || path.empty() )  {
    /// Help printout describing the basic command line interface
    cout <<
      "Usage: -plugin <name> -arg [-arg]                                      \n"
      "     name:   factory namedd4hep_GlobalAlignmentWriter                \n\n"
      "     -output <string>         Path to the output file generated.       \n"
      "     -path   <string>         Path to the detector element for which   \n"
      "                              the alignment file should be written.    \n"
      "     -transactions            Enable output transactions.              \n"
      "\tArguments given: " << arguments(argc,argv) << endl << flush;
    ::exit(EINVAL);
  }

  printout(ALWAYS,"AlignmentXmlWriter",
           "++ Writing dd4hep alignment constants of the \"%s\" DetElement tree to file \"%s\"",
           path.c_str(), output.c_str());
  top = detail::tools::findDaughterElement(description.world(),path);
  if ( top.isValid() )   {
    GlobalAlignmentWriter wr(description);
    return wr.write(wr.dump(top,enable_transactions), output);
  }
  except("AlignmentXmlWriter","++ Invalid top level detector element name: %s",path.c_str());
  return 1;
}
DECLARE_APPLY(DD4hep_GlobalAlignmentXmlWriter, create_global_alignment_xml_file)
