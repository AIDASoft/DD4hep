#include "TInterpreter.h"
void run()  {
  gInterpreter->ProcessLine(".X initAClick.C");
  gInterpreter->ProcessLine(".L dictionaries.C+");
  //gInterpreter->ProcessLine(".L xmlAClick.C+");
  gInterpreter->ProcessLine(".L TEve.C+");
}

