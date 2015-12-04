#include "TInterpreter.h"
#include "TSystem.h"
#include "RVersion.h"
#include <iostream>
#include <string>

using namespace std;

string make_str(const char* data)  {
  if ( !data )   {
    cout << "make_str:  '" << (data ? data : "Bad-Pointer") << "'" << endl;
    return string("");
  }
  return string(data);
}

int processCommand(const char* command, bool end_process)   {
  int status;
  // Disabling auto-parse is a hack required by a bug in ROOT
  gInterpreter->SetClassAutoparsing(false);
  status = gInterpreter->ProcessLine(command);
  gInterpreter->SetClassAutoparsing(true);
  ::printf("+++ Status(%s) = %d\n",command,status);
  if ( end_process )  {
    gInterpreter->ProcessLine("gSystem->Exit(0)");
  }
  return status;
}

int processMacro(const char* macro, bool end_process)   {
  int status;
  string cmd = ".X ";
  cmd += macro;
  cmd += ".C+()";
  return processCommand(cmd.c_str(), end_process);
}

int initAClick(const char* command=0)  {
  string rootsys = make_str(gSystem->Getenv("ROOTSYS"));
  string g4_base = make_str(gSystem->Getenv("G4INSTALL"));
  string dd4hep  = make_str(gSystem->Getenv("DD4hepINSTALL"));
  string libs = (" -L"+rootsys+"/lib");
  string inc  = " -I"+dd4hep+"/examples/DDG4/examples" +
    " -I"+dd4hep +
    " -I"+dd4hep+"/include" +
    " -I"+g4_base+"/include/Geant4" +
    " -Wno-shadow -g -O0";
  if ( ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0) )
    libs += " -lCore -lMathCore";
  else
    libs += " -lCore -lCint -lMathCore";
  libs += " -pthread -lm -ldl -rdynamic";
  libs += " -L"+dd4hep+"/lib -lDDCore -lDDG4 -lDDSegmentation";
  libs += (" -L"+g4_base+"/lib -L"+g4_base+"/lib64 -lG4event -lG4tracking -lG4particles");
  gSystem->AddIncludePath(inc.c_str());
  gSystem->AddLinkedLibs(libs.c_str());
  cout << "+++ Includes:   " << gSystem->GetIncludePath() << endl;
  cout << "+++ Linked libs:" << gSystem->GetLinkedLibs()  << endl;
  int ret = gSystem->Load("libDDG4Plugins");
  if ( 0 == ret )   {
    if ( command )  {
      processCommand(command, true);
    }
  }
  return ret;
}
