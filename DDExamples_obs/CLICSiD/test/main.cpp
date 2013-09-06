#include "DD4hep/LCDD.h"
//#include "TGDMLWrite.h"
#include "TGeoManager.h"
#include <iostream>
#include <vector>
#include <string>


using namespace std;
using namespace DD4hep;

Geometry::LCDD& compact2geo(int argc, char **argv)  {
  string input;
#ifdef _WIN32
  input = "file:../cmt/compact.xml";
#else
  //input = "file:/afs/cern.ch/user/f/frankb/scratch0/ONLINE/ONLINE_HEAD/Online/GeoTest/cmt/compact.xml";

  input = "compact.xml";
  //input = "file://../cmt/compact.xml";
  //input = "http://www.cern.ch/frankm/compact.xml";

#endif
  if ( argc>1 ) {
    input = argv[1];
  }
  //Geometry::LCDDImp *lcdd = new Geometry::LCDDImp;
  Geometry::LCDD& lcdd = Geometry::LCDD::getInstance();  
  cout << argc << " Input file : " << input << endl;
  lcdd.fromCompact(input);
  lcdd.dump();
  return lcdd;
}

#include "TRint.h"
//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  vector<char*> args;

  for(int i=0; i<argc;++i) args.push_back((char*)argv[i]);

  // Create an interactive ROOT application
  int     r_argc = 0;
  char**  r_argv = 0; 
  TRint *theApp = new TRint("Rint", &r_argc, r_argv);
  
  Geometry::LCDD& lcdd = compact2geo((int)args.size(),&args[0]);
  //TGDMLWrite wr;
  //wr.WriteGDMLfile(gGeoManager,"ILCEx.gdml","");

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
