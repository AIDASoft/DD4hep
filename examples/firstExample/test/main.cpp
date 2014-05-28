#include "DD4hep/LCDD.h"
//#include "TGDMLWrite.h"
#include "TGeoManager.h"
#include <iostream>
#include <vector>
#include <string>


using namespace std;
using namespace DD4hep;
using namespace Geometry;


#include "TRint.h"
//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  vector<char*> args;

  for(int i=0; i<argc;++i) args.push_back((char*)argv[i]);

  // Create an interactive ROOT application
  int     r_argc = 0;
  char**  r_argv = 0; 
  TRint *theApp = new TRint("Rint", &r_argc, r_argv);
  
  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact(args[1]);

  lcdd.dump();  
  // and enter the event loop...
  theApp->Run();
  delete theApp;
  return 0;
}

int main(int argc,char** argv)  {
  //return read_compact();
  //return compact2lcdd();
  return run_interpreter(argc,argv);
}
