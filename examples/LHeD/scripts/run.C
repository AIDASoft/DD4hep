
// ROOT include files
#include "TInterpreter.h"

// C/C++ include files
#include <string>

// Run a ROOT macro
void run(const char* macro)  {
  int status;
  char cmd[1024];
  const char* dd4hep_install = getenv("DD4hepINSTALL");
  const char* dd4hep = getenv("DD4hep");
  if ( dd4hep_install )  {
    ::sprintf(cmd,".L %s/examples/LHeD/scripts/initAClick.C+",dd4hep_install);
  }
  else  {
    ::sprintf(cmd,".L examples/LHeD/scripts/initAClick.C+");
  }
  status = gInterpreter->ProcessLine(cmd); 
  ::printf("Status(%s) = %d\n",cmd,status);
  status = gInterpreter->ProcessLine("initAClick()");
  ::printf("Status(%s) = %d\n",cmd,status);
  ::sprintf(cmd,"processMacro(\"%s\",true)",macro);
  status = gInterpreter->ProcessLine(cmd);
  ::printf("Status(%s) = %d\n",cmd,status);
}
