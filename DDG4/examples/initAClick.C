#include "TSystem.h"
#include <iostream>
#include <string>

using namespace std;

void init()  {
  string base = gSystem->Getenv("SW");
  string inc = " -I"+base+"/DD4hep/DD4hep.trunk/";
  gSystem->AddIncludePath((inc + "DDSegmentation/include ").c_str());
  gSystem->AddIncludePath((inc + "DDCore/include ").c_str());
  gSystem->AddIncludePath((inc + "DDG4/include ").c_str());
  gSystem->AddIncludePath((" -I" + base + "/geant4/include/Geant4 -Wno-shadow -g -O0 ").c_str());
  string lib = " -L"+base+"/";
  gSystem->AddLinkedLibs((lib+"geant4/lib -lG4event -lG4tracking -lG4particles ").c_str());
  gSystem->AddLinkedLibs((lib+"DD4hep/build/lib -lDD4hepCore -lDD4hepG4 -lDDSegmentation ").c_str());
  cout << "Includes:   " << gSystem->GetIncludePath() << endl;
  cout << "Linked libs:" << gSystem->GetLinkedLibs()  << endl;
}

void initAClick()  {
  init();
}

