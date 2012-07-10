//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
//====================================================================
//  Example application for using TPC functionality
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//  Reads in space points of energy deposits, e.g. from Geant4
//  simulation and maps those onto the pads of the TPC.
//
//====================================================================

#include "DD4hep/LCDD.h"
#include <iostream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
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
  TFile *out_file = new TFile("TPCDigitisation_Output.root","RECREATE");
  //vectors for output file
  int EvID;
  std::vector<int> moduleIDs;
  std::vector<int> padIDs;
  std::vector<double> zPositions;
  std::vector<double> chargeDeposition;
  //Tree structure
  TTree *out_tree = new TTree("PadTree","PadTree");
  out_tree->Branch("EvID",&EvID);
  out_tree->Branch("moduleIDs",&moduleIDs) ;
  out_tree->Branch("padIDs",&padIDs) ;
  out_tree->Branch("zPositions",&zPositions) ;
  out_tree->Branch("chargeDeposition",&chargeDeposition);
  
  //read in File with space points of energy deposits
  TFile *input=new TFile("TPCSimulation_Output.root","READ");
  int EvID_in;
  std::vector<double> *xPos=0;
  std::vector<double> *yPos=0;
  std::vector<double> *zPos=0;
  std::vector<double> *charge=0;
  TTree *sim_data = (TTree*)gDirectory->Get("SimTree");
  // Set branch addresses.
  sim_data->SetBranchAddress("EvID",&EvID_in);			 
  sim_data->SetBranchAddress("xPos",&xPos) ;		 
  sim_data->SetBranchAddress("yPos",&yPos) ;		 
  sim_data->SetBranchAddress("zPos",&zPos) ;		 
  sim_data->SetBranchAddress("charge",&charge) ;		 
  
  //container to hold the count of pads hit
  //map<pair<moduleID,padID>,vector<z,Q>
  std::map< std::pair<int,int> , std::pair<double,double>  > padmap;
  map< std::pair<int,int> , std::pair<double,double> >::iterator it;

  //Event Loop
  for(int i=0;i<sim_data->GetEntries();i++)
    {
      //pad loop
      sim_data->GetEntry(i);
      //loop the spacepoints x,y,z,E
      for(int p=0;p<xPos->size();p++)
	{
	  double x=(*xPos)[p];
	  double y=(*yPos)[p];
	  double z=(*zPos)[p];
	  double E=(*charge)[p];

	  int endplate=0;
	  if(z<0)
	    endplate=1;
	  //check if point is over module. If not move to next point
	  bool measured=false;
	  try{
	    measured=tpc.isInsideModule(x,y,endplate);
	  }
	  catch(OutsideGeometryException e){};
	  if(!measured)
	    continue;
	  TPCModule mymod=tpc.getNearestModule(x,y,endplate);
	  int modID=mymod.getID();
	  int padID= mymod.getNearestPad(x,y);
	  
	  std::pair<int,int> ID_pair=make_pair(modID,padID);
	  
	  //check if that pad has already been hit
	  it = padmap.find(ID_pair);
	  if(it!=padmap.end())
	    {
	      double new_z=(it->second.first*it->second.second+z*E)/(it->second.second+E);
	      double new_E=it->second.second+E;
	      padmap[ID_pair]=make_pair(new_z,new_E);
	    }
	  else //create a new entry
	    {
	      std::pair<double,double>  hit=make_pair(z,E);
	      padmap[ID_pair]=hit;
	    }
	}//point loop

      for ( it=padmap.begin() ; it != padmap.end(); it++ )
	{
	  moduleIDs.push_back(it->first.first);
	  padIDs.push_back(it->first.second);
	  zPositions.push_back(it->second.first);
	  chargeDeposition.push_back(it->second.second);
	}
      EvID=EvID_in;
      out_tree->Fill();
      //clean up for next event
      moduleIDs.clear();
      padIDs.clear();
      zPositions.clear();
      chargeDeposition.clear();
      padmap.clear();
    }

  //event loop ends, write file
  out_file->Write();

  return 0;
}
