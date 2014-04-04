#include "TInterpreter.h"
#include "TSystem.h"
#include <iostream>
#include <string>

using namespace std;

string make_str(const char* data)  {
  cout << "make_str:  '" << (data ? data : "Bad-Pointer") << "'" << endl;
  return string(data);
}

void initAClick(const char* command=0)  {
  string rootsys = make_str(gSystem->Getenv("ROOTSYS"));
  string g4_base = make_str(gSystem->Getenv("G4INSTALL"));
  string dd4hep  = make_str(gSystem->Getenv("DD4hepINSTALL"));
  string inc     = " -I"+dd4hep+"/include -I"+g4_base+"/include/Geant4 -Wno-shadow -g -O0";
  string libs = (" -L"+rootsys+"/lib");
  libs += " -lCore -lCint -lMathCore -L"+dd4hep+"/lib -lDD4hepCore -lDD4hepG4 -lDDSegmentation";
  libs += (" -L"+g4_base+"/lib -L"+g4_base+"/lib64 -lG4event -lG4tracking -lG4particles");
  gSystem->AddIncludePath(inc.c_str());
  gSystem->AddLinkedLibs(libs.c_str());
  cout << "Includes:   " << gSystem->GetIncludePath() << endl;
  cout << "Linked libs:" << gSystem->GetLinkedLibs()  << endl;

  string ddg4_examples = string(getenv("DD4hepINSTALL"))+"/examples/DDG4/examples";
  string cmd = ".L "+ddg4_examples+"/dictionaries.C+";
  //gInterpreter->ProcessLine(cmd.c_str());
  if ( command )   {
    string cmd = command;
    cout << "Executing command:" << cmd << endl;
    gInterpreter->ProcessLine(cmd.c_str());
  }
}
