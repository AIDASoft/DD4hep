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

// Framework include files
// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cerrno>
#include <string>
//________________________________________________________________________________
#include "TRint.h"
#include "TGeoManager.h"

using namespace dd4hep;

//________________________________________________________________________________
namespace {
  //______________________________________________________________________________
  void usage_default(const char* name) {
    std::cout << " " << name << " -opt [-opt]                                             \n"
      "        -compact       <file>       Specify the compact geometry file              \n"
      "                     [REQUIRED]     At least one compact geo file is required!     \n"
              << std::endl;
    std::exit(EINVAL);
  }

  //______________________________________________________________________________
  struct Args  {
    bool        volmgr, dry_run, destroy, interpreter, ui;
    PrintLevel  print;
    std::vector<const char*> geo_files, build_types;
    std::vector<std::vector<const char*> > plugins;

    //____________________________________________________________________________
    Args() {
      ui          = false;
      volmgr      = false;
      dry_run     = false;
      destroy     = false;
      interpreter = true;
      print       = INFO;
    }

    //____________________________________________________________________________
    int handle(int& i, int argc, char** argv)    {
      if ( ::strncmp(argv[i],"-compact",5)==0 || ::strncmp(argv[i],"-input",4)==0 )  {
        geo_files.push_back(argv[++i]);
        if ( argc>i+2 && ::strncmp(argv[i+1],"-build_type",6)==0 )  {
          build_types.push_back(argv[i+2]);
          i += 2;
        }
        else  {
          build_types.push_back("BUILD_DEFAULT");
        }
      }
      else if ( ::strncmp(argv[i],"-load_only",5)==0 )
        dry_run = true;
      else if ( ::strncmp(argv[i],"-dry-run",5)==0 )
        dry_run = true;
      else if ( ::strncmp(argv[i],"-print",4)==0 )
        setPrintLevel(PrintLevel(print = decodePrintLevel(argv[++i])));
      else if ( ::strncmp(argv[i],"-destroy",5)==0 )
        destroy = true;
      else if ( ::strncmp(argv[i],"-no-destroy",8)==0 )
        destroy = false;
      else if ( ::strncmp(argv[i],"-volmgr",4)==0 )
        volmgr = true;
      else if ( ::strncmp(argv[i],"-no-volmgr",7)==0 )
        volmgr = false;
      else if ( ::strncmp(argv[i],"-interpreter",6)==0 )
        interpreter = true;
      else if ( ::strncmp(argv[i],"-no-interpreter",7)==0 )
        interpreter = false;
      else if ( ::strncmp(argv[i],"-interactive",6)==0 )
        interpreter = true;
      else if ( ::strncmp(argv[i],"-no-interactive",7)==0 )
        interpreter = false;
      else if ( ::strncmp(argv[i],"-ui",3)==0 )
        ui = true;
      else if ( ::strncmp(argv[i],"-no-ui",6)==0 )
        ui = false;
      else if ( ::strncmp(argv[i],"-plugin",5)==0 )   {
        // Need to interprete plugin args here locally.....
        plugins.push_back(std::vector<const char*>());
        plugins.back().push_back(argv[++i]);
        for( ++i; i < argc; ++i )   {
          if ( ::strncmp(argv[i],"-plugin",5)==0 ) { --i; break; }
          if ( ::strncmp(argv[i],"-end-plugin",4)==0 )  { break; }
          plugins.back().push_back(argv[i]);
        }
        plugins.back().push_back(0);
      }
      else 
        return 0;
      return 1;
    }
  };
}

//______________________________________________________________________________
int main(int argc,char** argv)  {
  Args args;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( args.handle(i,argc,argv) )
        continue;
      else
        usage_default("Blas");
    }
    else {  // This is the default
      args.geo_files.push_back(argv[i]);
      args.build_types.push_back("BUILD_DEFAULT");
    }
  }
  std::vector<Detector*> detector_instances;
  TStorage::EnableStatistics(10000);
  // Load all compact files
  for(size_t i=0; i<args.geo_files.size(); ++i)  {
    const char* arg[] = {args.geo_files[i], args.build_types[i], 0};
    printout(ALWAYS,"MultiGeom","=============================================================================");
    printout(ALWAYS,"MultiGeom","Loading geometry: %s",arg[0]);
    Detector& description = Detector::getInstance(arg[0]);
    detector_instances.push_back(&description);
    description.apply("DD4hep_XMLLoader",2,(char**)arg);
  }
  int cnt = 0;
  for(auto d : detector_instances)   {
    char line[256];
    printout(ALWAYS,"MultiGeom","=============================================================================");
    printout(ALWAYS,"MultiGeom","Manager: %s",d->manager().GetName());
    printout(ALWAYS,"MultiGeom","Manager: %p",&d->manager());
    ::snprintf(line,sizeof(line),"TGeoManager* mgr%d = (TGeoManager*)%p",cnt,(void*)&d->manager());
    printout(ALWAYS,"MultiGeom","%s",line);
    gInterpreter->ProcessLine(line);
    ::snprintf(line,sizeof(line),"TGeoVolume* top%d = (TGeoVolume*)%p",cnt,(void*)d->manager().GetTopVolume());
    printout(ALWAYS,"MultiGeom","%s",line);
    gInterpreter->ProcessLine(line);
    printout(ALWAYS,"MultiGeom","=============================================================================");
    ++cnt;
  }
  if ( args.interpreter )   {
    std::pair<int, char**> a(0,0);
    gGeoManager = 0;
    TRint app("DD4hep", &a.first, a.second);
    app.Run(); // May never return!
  }
  for(auto d : detector_instances)   {
    printout(ALWAYS,"MultiGeom","=============================================================================");
    printout(ALWAYS,"MultiGeom","DELETE Geometry: %s",d->manager().GetName());
    delete d;
  }
  return 0;
}
