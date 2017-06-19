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
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// Framework include files
#include "run_plugin.h"

//______________________________________________________________________________
namespace {
  void usage() {
    cout << "geoConverter -opt [-opt]                                                \n"
      "        Action flags:               Usage is exclusive, 1 required!           \n"
      "        -compact2description               Convert compact xml geometry to description.     \n"
      "        -compact2gdml               Convert compact xml geometry to gdml.     \n"
      "        -compact2pandora            Convert compact xml to pandora xml        \n"
      "        -compact2vis                Convert compact xml to visualisation attrs\n\n"
      "        -input  <file>  [REQUIRED]  Specify input file.                       \n"
      "        -output <file>  [OPTIONAL]  Specify output file.                      \n"
      "                                    if no output file is specified, the output\n"
      "                                    device is stdout.                         \n"
      "        -ascii          [OPTIONAL]  Dump visualisation attrs in csv format.   \n"
      "                                    [Only valid for -compact2vis]             \n"
      "        -destroy        [OPTIONAL]  Force destruction of the Detector instance    \n"
      "                                    before exiting the application            \n"
      "        -volmgr         [OPTIONAL]  Load and populate phys.volume manager to  \n"
      "                                    check the volume ids for duplicates etc.  \n"
         << endl;
    exit(EINVAL);
  }


  //______________________________________________________________________________
  int invoke_converter(int argc,char** argv)  {
    bool ascii = false;
    bool volmgr = false;
    bool destroy      = false;
    bool compact2description = false;
    bool compact2gdml = false;
    bool compact2pand = false;
    bool compact2vis  = false;
    int output = 0;
    vector<char*> geo_files;
    for(int i=1; i<argc;++i) {
      if ( argv[i][0]=='-' ) {
        if ( strncmp(argv[i],"-compact2description",12)==0 )
          compact2description = true;
        else if ( strncmp(argv[i],"-compact2gdml",12)==0 )
          compact2gdml = true;
        else if ( strncmp(argv[i],"-compact2pandora",12)==0 )
          compact2pand = true;
        else if ( strncmp(argv[i],"-compact2vis",12)==0 )
          compact2vis = true;
        else if ( strncmp(argv[i],"-input",2)==0 )
          geo_files.push_back(argv[++i]);
        else if ( strncmp(argv[i],"-output",2)==0 )
          output = ++i;
        else if ( strncmp(argv[i],"-ascii",5)==0 )
          ascii = true;
        else if ( strncmp(argv[i],"-destroy",2)==0 )
          destroy = true;
        else if ( strncmp(argv[i],"-volmgr",2)==0 )
          volmgr = true;
        else
          usage();
      }
      else {
        usage();
      }
    }
    if ( geo_files.empty() || (!compact2description && !compact2gdml && !compact2pand && !compact2vis))
      usage();

    Detector& description = dd4hep_instance();
    // Load compact files
    for(size_t i=0; i<geo_files.size(); ++i)  {
      const char* plugin_argv[] = {geo_files[i], 0};
      run_plugin(description,"dd4hepCompactLoader",1,(char**)plugin_argv);
    }
    // Create volume manager and populate it required
    if ( volmgr  ) run_plugin(description,"dd4hepVolumeManager",0,0);
    // Execute data converter.
    if ( compact2description )
      run_plugin(description,"dd4hepGeometry2Detector",output,&argv[output]);
    else if ( compact2gdml )
      run_plugin(description,"dd4hepGeometry2GDML",output,&argv[output]);
    else if ( compact2pand )
      run_plugin(description,"dd4hepGeometry2PANDORA",output,&argv[output]);
    else if ( compact2vis && ascii )
      run_plugin(description,"dd4hepGeometry2VISASCII",output,&argv[output]);
    else if ( compact2vis )
      run_plugin(description,"dd4hepGeometry2VIS",output,&argv[output]);
    if ( destroy ) delete &description;
    return 0;
  }
}

/// Main entry point as a program
int main(int argc, char** argv)   {
  try  {
    return invoke_converter(argc, argv);
  }
  catch(const std::exception& e)  {
    std::cout << "Got uncaught exception: " << e.what() << std::endl;
  }
  catch (...)  {
    std::cout << "Got UNKNOWN uncaught exception." << std::endl;
  }
  return EINVAL;    
}
