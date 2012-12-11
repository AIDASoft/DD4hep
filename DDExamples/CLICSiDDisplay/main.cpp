#include "DD4hep/LCDD.h"
//#include "TGDMLWrite.h"
#include "TGeoManager.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "TRint.h"

using namespace std;
using namespace DD4hep;

//______________________________________________________________________________
void usage() {
  cout << "<exe> -opt [-opt]                 \n"
    "        -compact       <file>       Compact geometry file                     \n"
    "                                    At least one compact geo file is required!\n"
    "        -gdml          <file>       GDML output file of the compact geometry  \n"
    "                                    Flag switches display off.                \n"
    "        -display                    Reenable display when writing GDML file.  \n"
       << endl;
  exit(1);
}

//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  bool run_display = true;
  bool run_gdml    = false;
  string gdml_file;
  vector<char*> geo_files;
  for(int i=1; i<argc;++i) {
    if ( argv[i][0]=='-' ) {
      if ( strncmp(argv[i],"-compact",5)==0 )   {
	geo_files.push_back((char*)argv[++i]);
      }
      else if ( strncmp(argv[i],"-gdml",5)==0 )   {
	gdml_file = argv[++i];
	run_display = false;
	run_gdml = true;
      }
      else if ( strncmp(argv[i],"-display",5)==0 )   {
	run_display = true;
      }
    }
    else {  // This is the default
      geo_files.push_back((char*)argv[i]);
    }
  }
  if ( !geo_files.size() > 0 ) {
    usage();
  }

  Geometry::LCDD& lcdd = Geometry::LCDD::getInstance();  
  for(size_t j=0; j<geo_files.size(); ++j) {
    string input = geo_files[j];
    cout << "Input file : " << input << endl;
    lcdd.fromCompact(input);
  }
  if ( run_gdml )   {
    char* args[] = {(char*)gdml_file.c_str(),0};
    lcdd.apply("gdml_converter",1,args);
  }
  if ( run_display )   {
    // Create an interactive ROOT application
    int     r_argc = 0;
    char**  r_argv = 0; 
    TRint *theApp = new TRint("Rint", &r_argc, r_argv);
    lcdd.dump();
    //TGDMLWrite wr;
    //wr.WriteGDMLfile(gGeoManager,"ILCEx.gdml","");    
    // and enter the event loop...
    theApp->Run();
    delete theApp;
  }
  return 0;
}

int main(int argc,char** argv)  {
  return run_interpreter(argc,argv);
}
