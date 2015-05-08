#include "TInterpreter.h"
void run()  {
  gInterpreter->ProcessLine(".X initAClick.C");
  //gInterpreter->ProcessLine(".L exampleAClick.C+");
  gInterpreter->ProcessLine(".L xmlAClick.C+");
  //gInterpreter->ProcessLine(".L TEve.C+");
  //gSystem->Load("libDDCore");
  //gSystem->Load("libDDG4");
  //gInterpreter->ProcessLine(".L FCC_Hcal.C+");
  //gInterpreter->ProcessLine(".X DDG4Dump.C+");
  //gInterpreter->ProcessLine(".X a.C++");
}

