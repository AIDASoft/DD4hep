#include "TSystem.h"
#include <iostream>
#include <string>

using namespace std;
string make_str(const char* data)  {
  cout << "make_str:  '" << (data ? data : "Bad-Pointer") << "'" << endl;
  return string(data);
}

void init()  {
  string g4_base = make_str(gSystem->Getenv("Geant4_DIR"));
  string base    = make_str(gSystem->Getenv("SW"));
  string inc     = " -I"+make_str(gSystem->Getenv("DD4hep_DIR"))+"/../DD4hep.trunk/";
  gSystem->AddIncludePath((inc + "DDSegmentation/include ").c_str());
  gSystem->AddIncludePath((inc + "DDCore/include ").c_str());
  gSystem->AddIncludePath((inc + "DDG4/include ").c_str());
  gSystem->AddIncludePath((" -I" + g4_base + "/include/Geant4 -Wno-shadow -g -O0 ").c_str());
  string g4_libs = " -L"+g4_base+"/lib -L"+g4_base+"/lib64";
  string libs = " -L"+make_str(gSystem->Getenv("DD4hep_DIR"))+"/lib";
  gSystem->AddLinkedLibs((libs+" -lDD4hepCore -lDD4hepG4 -lDDSegmentation ").c_str());
  gSystem->AddLinkedLibs((g4_libs+" -lG4event -lG4tracking -lG4particles ").c_str());
  cout << "Includes:   " << gSystem->GetIncludePath() << endl;
  cout << "Linked libs:" << gSystem->GetLinkedLibs()  << endl;
}

void initAClick()  {
  init();
}

