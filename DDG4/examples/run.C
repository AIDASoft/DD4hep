#include "TInterpreter.h"
void run()  {
  gInterpreter->ProcessLine(".X initAClick.C");
  //gInterpreter->ProcessLine(".L dictionaries.C+");
  //gInterpreter->ProcessLine(".L exampleAClick.C+");
  //gInterpreter->ProcessLine(".L xmlAClick.C+");
  //gInterpreter->ProcessLine(".L TEve.C+");
  //gSystem->Load("libDD4hepCore");
  //gSystem->Load("libDD4hepG4");
  gInterpreter->ProcessLine(".L FCC_Hcal.C+");
  //gInterpreter->ProcessLine(".X DDG4Dump.C+");
  //gInterpreter->ProcessLine(".X a.C++");
}

