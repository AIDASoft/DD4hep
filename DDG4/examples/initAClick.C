#include "TSystem.h"
#include <iostream>
#include <string>

using namespace std;
string make_str(const char* data)  {
  cout << "make_str:  '" << (data ? data : "Bad-Pointer") << "'" << endl;
  return string(data);
}

void initAClick()  {
  string g4_base = make_str(gSystem->Getenv("Geant4_DIR"));
  string dd4hep  = make_str(gSystem->Getenv("DD4hep_DIR"));
  string inc     = " -I"+dd4hep+"/include -I"+g4_base+"/include/Geant4 -Wno-shadow -g -O0";
  string libs = " -L"+dd4hep+"/lib -lDD4hepCore -lDD4hepG4 -lDDSegmentation";
  libs += (" -L"+g4_base+"/lib -L"+g4_base+"/lib64 -lG4event -lG4tracking -lG4particles");
  gSystem->AddIncludePath(inc.c_str());
  gSystem->AddLinkedLibs(libs.c_str());
  cout << "Includes:   " << gSystem->GetIncludePath() << endl;
  cout << "Linked libs:" << gSystem->GetLinkedLibs()  << endl;
}
