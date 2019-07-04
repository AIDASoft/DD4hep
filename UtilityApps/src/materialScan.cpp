//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//
//  Simple program to print all the materials in a detector on
//  a straight line between two given points
// 
//  Author     : F.Gaede, DESY
//  Author     : M.Frank, CERN
//
//==========================================================================

#include "TError.h"
#include "TInterpreter.h"

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialScan.h"
#include "main.h"

using namespace dd4hep;
using namespace dd4hep::rec;

int main_wrapper(int argc, char** argv)   {
  struct Handler  {
    Handler() { SetErrorHandler(Handler::print); }
    static void print(int level, Bool_t abort, const char *location, const char *msg)  {
      if ( level > kInfo || abort ) ::printf("%s: %s\n", location, msg);
    }
    static void usage()  {
      std::cout << " usage: materialScan compact.xml x0 y0 z0 x1 y1 z1 [-interactive]" << std::endl 
                << "        -> prints the materials on a straight line between the two given points ( unit is cm) " << std::endl
                << "        -interactive   Load geometry once, then allow for shots from the ROOT prompt"
                << std::endl;
      exit(1);
    }
  } _handler;

  if( argc < 8 ) Handler::usage();
  bool interactive = argc == 9 && ::strncmp(argv[8],"-interactive",5) == 0;
  double x0, y0, z0, x1, y1, z1;
  std::string inFile =  argv[1];
  std::stringstream sstr;
  sstr << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] << " " << "NONE";
  sstr >> x0 >> y0 >> z0 >> x1 >> y1 >> z1;
  if ( !sstr.good() ) Handler::usage();

  setPrintLevel(WARNING);
  Detector& description = Detector::getInstance();
  description.fromXML(inFile);
  MaterialScan scan(description);
  scan.print(x0, y0, z0, x1, y1, z1);
  if ( interactive )   {
    char cmd[256];
    description.apply("DD4hep_InteractiveUI",0,0);
    ::snprintf(cmd,sizeof(cmd),
               "dd4hep::rec::MaterialScan& gMaterialScan = "
               "*(dd4hep::rec::MaterialScan*)%p",(void*)&scan);
    gInterpreter->ProcessLine(cmd);
    printout(ALWAYS,"materialScan","Use the ROOT interpreter variable gMaterialScan to interact with the material scanner");
    gInterpreter->ProcessLine(".class dd4hep::rec::MaterialScan");
    description.apply("DD4hep_Rint",0,0);
  }
  return 0;
}
