//
//  pymain.cpp
//  
//
//  Created by Pere Mato on 20/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "LCDD.h"
#include <iostream>
#include <vector>
#include <string>
#include "TRint.h"

using namespace std;
using namespace DetDesc;
using namespace Geometry;

//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  vector<char*> args;
  
  for(int i=0; i<argc;++i) args.push_back((char*)argv[i]);
  argc = 0;
  
  // Create an interactive ROOT application
  TRint *theApp = new TRint("Rint", &argc, argv, 0, 0, kTRUE);
  
  LCDD& lcdd = LCDD::getInstance();
  switch (args.size()) {
    case 1:
      lcdd.fromCompact();
      break;
    case 2:
      lcdd.fromCompact(args[1]);
      break;
    default:
      break;
  }
  
  lcdd.dump();
    
  // and enter the event loop...
  theApp->Run();
  delete theApp;
  return 0;
}

int main(int argc,char** argv)  {
  return run_interpreter(argc,argv);
}
