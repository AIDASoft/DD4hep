//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
//====================================================================
//  Demonstrator application for using TPC functionality 
//  for hit reconstruction
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//  Reads in pads with charge and reconstructs 3D space points.
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
 
  //Output file containing the hits
  TFile *out_file = new TFile("TPCHitReco_Output.root","RECREATE");
  //vectors for output file
  int EvID;
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  std::vector<double> charge;
  //Tree structure
  TTree *out_tree = new TTree("HitTree","HItTree");
  out_tree->Branch("EvID",&EvID);
  out_tree->Branch("xPos",&xPos) ;
  out_tree->Branch("yPos",&yPos) ;
  out_tree->Branch("zPos",&zPos) ;
  out_tree->Branch("charge",&charge);
  
  //read in File with space points of energy deposits
  TFile *input=new TFile("TPCDigitisation_Output.root","READ");
  int EvID_in;
  std::vector<int> *moduleIDs=0;
  std::vector<int> *padIDs=0;
  std::vector<double> *zPositions=0;
  std::vector<double> *chargeDeposition=0;
  
  TTree *pad_data = (TTree*)gDirectory->Get("PadTree");
  // Set branch addresses.
  pad_data->SetBranchAddress("EvID",&EvID_in);			 
  pad_data->SetBranchAddress("moduleIDs",&moduleIDs) ;		 
  pad_data->SetBranchAddress("padIDs",&padIDs) ;			 
  pad_data->SetBranchAddress("zPositions",&zPositions) ;		 
  pad_data->SetBranchAddress("chargeDeposition",&chargeDeposition);

  //Fill pads again in map for easier search of neighbours
  // on map per endplate
  //map<pair<moduleID,padID>,vector<z,Q>
  std::map< std::pair<int,int> , std::pair<double,double>  > padmap_EP1;
  map< std::pair<int,int> , std::pair<double,double> >::iterator it1;
  std::map< std::pair<int,int> , std::pair<double,double>  > padmap_EP2;
  map< std::pair<int,int> , std::pair<double,double> >::iterator it2;
    
  //Event Loop
  for(int i=0;i<pad_data->GetEntries();i++)
    {
      //pad loop
      pad_data->GetEntry(i);

      for(int p=0;p<moduleIDs->size();p++)
	{
	  std::pair<int,int> ID_pair=make_pair((*moduleIDs)[p],(*padIDs)[p]);
	  std::pair<double,double>  hit=make_pair((*zPositions)[p],(*chargeDeposition)[p]);
	  if((*zPositions)[p] > 0)
	    padmap_EP1[ID_pair]=hit;
	  else
	    padmap_EP2[ID_pair]=hit;
	}
   
      //hit reco goes per module per row
      //positive endplate
      for ( it1=padmap_EP1.begin() ; it1 != padmap_EP1.end(); it1++ )
	{
	  TPCModule mymod=tpc.getModule(it1->first.first,0);
	  cout << it1->first.first << "\t" <<(*it1).first.second <<" "<<mymod.getNPads()<< endl;
	  std::vector<double> center=mymod.getPadCenter(it1->first.second);
	   
	  xPos.push_back(center[0]);
	  yPos.push_back(center[1]);
	  zPos.push_back(it1->second.first);
	  charge.push_back(it1->second.second);
	}
     //negative endplate
      for ( it2=padmap_EP2.begin() ; it2 != padmap_EP2.end(); it2++ )
	{
	  std::vector<double> center=tpc.getModule(it2->first.first,1).getPadCenter(it2->first.second);
	  
	  xPos.push_back(center[0]);
	  yPos.push_back(center[1]);
	  zPos.push_back(it2->second.first);
	  charge.push_back(it2->second.second);
	}
      EvID=EvID_in;
      out_tree->Fill();
      //clean up for next event
      xPos.clear();
      yPos.clear();
      zPos.clear();
      charge.clear();
      padmap_EP1.clear();
      padmap_EP2.clear();
    }
  
  //event loop ends, write file
  out_file->Write();

  return 0;
}
