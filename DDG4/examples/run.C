#include "TInterpreter.h"
#include <string>
void run(const char* macro)  {
  char cmd[1024];
  const char* dd4hep_install = getenv("DD4hepINSTALL");
  if ( dd4hep_install )  {
    ::sprintf(cmd,".L %s/examples/DDG4/examples/initAClick.C+",dd4hep_install);
  }
  else  {
    ::sprintf(cmd,".L examples/DDG4/examples/initAClick.C+");
  }
  int status = gInterpreter->ProcessLine(cmd); 
  ::printf("Status(%s) = %d\n",cmd,status);
  status = gInterpreter->ProcessLine("initAClick()");
  ::printf("Status(%s) = %d\n",cmd,status);
  ::sprintf(cmd,"processMacro(\"%s\",true)",macro);
  status = gInterpreter->ProcessLine(cmd);
  ::printf("Status(%s) = %d\n",cmd,status);
}
