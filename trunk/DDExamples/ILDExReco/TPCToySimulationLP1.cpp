//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
//====================================================================
//  Fake hit generator to be replaced by G4 simulation
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
#include <TRandom.h>
#include "GearTPC.h"

using namespace std;
using namespace DD4hep;
using namespace Geometry;


int main(int argc,char** argv)  {
  
  // instanciate geometry from copact file
  LCDD& lcdd = LCDD::getInstance();
  lcdd.fromCompact(argv[1]);
  // get the TPC
  GearTPC tpc(lcdd.detector("TPC"));
 

  //Output file containing pads that are hit
  TFile *out_file = new TFile("TPCSimulation_Output.root","RECREATE");
  //vectors for output file
  int EvID;
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  std::vector<double> charge;
  std::vector<double> tpcPos=tpc.getPosition();
  //Tree structure
  TTree *out_tree = new TTree("SimTree","SimTree");
  out_tree->Branch("EvID",&EvID);
  out_tree->Branch("xPos",&xPos) ;
  out_tree->Branch("yPos",&yPos) ;
  out_tree->Branch("zPos",&zPos) ;
  out_tree->Branch("charge",&charge);

  TRandom *rndm = new TRandom();
  //event loop
  int NEVENTS=1000;
  int NPOINTS=500;

  for(int i=0;i<NEVENTS;i++)
    {
      //shoot along x different z positions
      double y=rndm->Uniform(-tpc.getOuterRadius(),tpc.getOuterRadius());
      double z=rndm->Uniform(tpcPos[2]-10,tpcPos[2]+10);
			     //point loop
      for (int p=0;p<NPOINTS;p++)
	{
	  double inner_r=0;
	  try{
	    inner_r=tpc.getInnerRadius();
	  }
	  catch(OutsideGeometryException e){};

	  double radius=2*tpc.getOuterRadius()*p/NPOINTS;
	  xPos.push_back(tpcPos[0]-tpc.getOuterRadius()+radius);
	  yPos.push_back(y);
	  zPos.push_back(z);
	  charge.push_back(rndm->Uniform(5,10));
	  //cout<<i<<"\t"<<p<<"\t"<<radius<<"\t"<<theta*180/M_PI<<"\t"<<sign_z*radius/tan(theta)<<"\t"<<tan(theta)<<endl;
	}
      EvID=i;
      out_tree->Fill();
      xPos.clear();
      yPos.clear();
      zPos.clear();
      charge.clear();
    }
  out_file->Write(); 
  return 0;
}
