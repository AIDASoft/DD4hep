//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
//====================================================================
//  Demonstrator application for using TPC functionality 
//  to calculate the binning of pads on modules
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================

#include "DD4hep/LCDD.h"
#include <iostream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include "GearTPC.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace Geometry;


int main(int argc,char** argv)  {
  
  // instanciate geometry from copact file
  LCDD& lcdd = LCDD::getInstance();
  lcdd.fromCompact(argv[1]);
  // get the TPC
  GearTPC tpc(lcdd.detector("TPC"));
 
  // for 5x10 pads
  double pad_height=10;
  double pad_width=5;
  std::vector<TPCModule> mymods=tpc.getModules(0);
  for(int m=0;m<mymods.size();m++)
    {
      TPCModule mymod=tpc.getModule(m,0);
      Tube       tube=mymod.volume().solid();
      double module_height= tube->GetRmax()-tube->GetRmin();
      double module_angle= tube->GetPhi2()-tube->GetPhi1();
      double pad_angle=pad_width/tube->GetRmin()*180/M_PI;
      cout << "Mod "<<m<<" r= " << module_height <<" , phi= "<< module_angle
	   <<"\t Binning theta= "<<module_height/pad_height
	   <<"   phi= "<<module_angle/pad_angle<<endl;
     }
  return 0;
}
