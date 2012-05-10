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
  //Event Loop
  
  double spacepoint[20][4];
  //test fill
  for(int p=0;p<10;p++)
    {
      spacepoint[p][0]=0;
      spacepoint[p][1]=tpc.getInnerRadius()+(tpc.getOuterRadius()-tpc.getInnerRadius())*p/10;
      spacepoint[p][2]=tpc.getMaxDriftLength()*p/10;
      spacepoint[p][3]=1;
    }
  for(int p=10;p<20;p++)
    {
      spacepoint[p][0]=spacepoint[p-10][0];
      spacepoint[p][1]=spacepoint[p-10][1];
      spacepoint[p][2]=spacepoint[p-10][2]+10;
      spacepoint[p][3]=4;
    }

  //container to hold the count of pads hit
  //map<pair<moduleID,padID>,vector<z,Q>
  std::map< std::pair<int,int> , std::pair<double,double>  > padmap;
  map< std::pair<int,int> , std::pair<double,double> >::iterator it;
  //loop the spacepoints x,y,z,E
  for(int p=0;p<20;p++)
    {
      double x=spacepoint[p][0];
      double y=spacepoint[p][1];
      double z=spacepoint[p][2];
      double E=spacepoint[p][3];

      int endplate=0;
      if(z<0)
	endplate=1;
      //check if point is over module. If not move to next point
      if(!tpc.isInsideModule(x,y,endplate))
	continue;
      TPCModule mymod=tpc.getNearestModule(x,y,endplate);
      int modID=mymod.getID();
      int padID= mymod.getNearestPad(x,y);
      cout<<p<<"\t"<<spacepoint[p][0]<<"\t"<<spacepoint[p][1]<<"\t"<<spacepoint[p][2]<<"\t"<<modID<<"\t"<<padID<<endl;  
      std::pair<int,int> ID_pair=make_pair(modID,padID);;

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
    }

  for ( it=padmap.begin() ; it != padmap.end(); it++ )
    {
      cout << it->first.first << "\t" <<(*it).first.second << "\t" << (*it).second.first <<"\t" << (*it).second.second << endl;
      moduleIDs.push_back(it->first.first);
      padIDs.push_back(it->first.second);
      zPositions.push_back(it->second.first);
      chargeDeposition.push_back(it->second.second);
    }
  out_tree->Fill();
  //clean up for next event
  moduleIDs.clear();
  padIDs.clear();
  zPositions.clear();
  chargeDeposition.clear();


  //event loop ends, write file
  out_file->Write();

  return 0;
}
