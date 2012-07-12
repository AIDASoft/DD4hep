//
//  pymain.cpp
//  
//
//  Created by Pere Mato on 20/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
//====================================================================
//  Test application for TPC functionality
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//
//====================================================================

#include "DD4hep/LCDD.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "GearTPC.h"
#include "TPCModuleData.h"


using namespace std;
using namespace DD4hep;
using namespace Geometry;

#include "TPCModuleData.h"


int main(int argc,char** argv)  {
  
  LCDD& lcdd = LCDD::getInstance();
  lcdd.fromCompact(argv[1]);

  ofstream myfile;
  myfile.open ("testOutput_TestTPCPrototype.txt");

  //based on the reference value precision input
  double myPrecision=1e-3;
  int failures=0;

  GearTPC tpc(lcdd.detector("TPC"));
  if(fabs(tpc.getOuterRadius()-500)>myPrecision)
    {
      myfile<<"FAILED tpc.getOuterRadius(): 500!="<<tpc.getOuterRadius()<<endl;
      failures++;
    }
  if(fabs(tpc.getMaxDriftLength()-497)>myPrecision)
    {
      myfile<<"FAILED tpc.getMaxDriftLength(): 497!="<<tpc.getMaxDriftLength()<<endl;
      failures++;
    }
  if(fabs(tpc.getEndPlateThickness(0)-1.5)>myPrecision)
    {
      myfile<<"FAILED tpc.getEndPlateThickness(0): 1.5!="<<tpc.getEndPlateThickness(0)<<endl;
      failures++;
    }
  if(fabs(tpc.getEndPlateZPosition(0)-998.5)>myPrecision)
    {
      myfile<<"FAILED tpc.getEndPlateZPosition(0): 998.5 !="<<tpc.getEndPlateZPosition(0)<<endl;
      failures++;
    }
  
  std::vector<TPCModule> mymods=tpc.getModules(0);
  if(fabs(mymods.size()-9)>myPrecision)
    {
      myfile<<"FAILED mymods.size(); 9!="<<mymods.size()<< endl;
      failures++;
    }
  int npads=0;
  for(int p=0;p<mymods.size();p++)
    npads+=tpc.getModule(p,0).getNPads();
  if(fabs(npads-11250)>myPrecision)
    {
      myfile<<"FAILED npads: 11250!=" <<npads<< endl;
      failures++;
    }
  if(fabs(tpc.isInsideModule(-150,0,0)-1)>myPrecision)
    {
      myfile<<"FAILED tpc.isInsideModule: 1!="<<tpc.isInsideModule(-150,0,0)<<endl;
      failures++;
    }
  if(fabs(tpc.getNearestModule(-150,0,0).getID()-3)>myPrecision)
    {
      myfile<<"FAILED tpc.getNearestModule(-150,0,0).getID(): 3!="<<tpc.getNearestModule(-150,0,0).getID()<<endl;
      failures++;
    }
  if(fabs(tpc.getNearestModule(800,0,0).getID()-2)>myPrecision)
    {
      myfile<<"FAILED tpc.getNearestModule(800,0,0).getID(): 2!=" <<tpc.getNearestModule(800,0,0).getID()<<endl;
      failures++;
    }


  TPCModule mymod=tpc.getModule(4,0);
  if(fabs(mymod.getID()-4)>myPrecision)
    {
      myfile<<"FAILED mymod.getID(): 4!=" <<mymod.getID()<<endl;
      failures++;
    }
  if(mymod.getPadType()!="cartesian_grid_xy")
    {
      myfile<<"FAILED mymod.getPadType(): cartesian_grid_xy!=" <<mymod.getPadType()<<endl;
      failures++;
    }
  if(fabs(mymod.getNPads()-1250)>myPrecision)
    {
      myfile<<"FAILED mymod.getNPads(): 1250!="<<mymod.getNPads()<<endl;
      failures++;
    }
  if(fabs(mymod.getNRows()-25)>myPrecision)
    {
      myfile<<"FAILED mymod.getNRows(): 25!="<<mymod.getNRows()<<endl;
      failures++;
    }
  if(fabs(mymod.getNPadsInRow(0)-50)>myPrecision)
    {
      myfile<<"FAILED mymod.getNPadsInRow(0): 50!="<<mymod.getNPadsInRow(0)<<endl;
      failures++;
    }
  if(fabs(mymod.getRowHeight(0)-6)>myPrecision)
    {
      myfile<<"FAILED mymod.getRowHeight(0): 6!="<<mymod.getRowHeight(0)<<endl;
      failures++;
    }
  if(fabs(mymod.getPadPitch(1)-2)>myPrecision)
    {
      myfile<<"FAILED mymod.getPadPitch(1): 2!="<<mymod.getPadPitch(1)<<endl;
      failures++;
    }
  if(fabs(mymod.getRowNumber(15)-0)>myPrecision)
    {
      myfile<<"FAILED mymod.getRowNumber(15): 0!=" <<mymod.getRowNumber(15)<<endl;
      failures++;
    }
  if(fabs(mymod.getPadNumber(15)-15)>myPrecision)
    {
      myfile<<"FAILED mymod.getPadNumber(15): 15!=" <<mymod.getPadNumber(15)<<endl;
      failures++;
    }
  if(fabs(mymod.getPadIndex(1,5)-55)>myPrecision)
    {
      myfile<<"FAILED mymod.getPadIndex(1,5): 55!=" <<mymod.getPadIndex(1,5)<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getRightNeighbour(0)-1)>myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getRightNeighbour(0): 1!="<<tpc.getModule(0,0).getRightNeighbour(0)<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getLeftNeighbour(3)-2)>myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getLeftNeighbour(3): 2!="<<tpc.getModule(0,0).getLeftNeighbour(3)<<endl;
      failures++;
    }
  std::vector<double> center1=tpc.getModule(0,0).getPadCenter(10);
  if(fabs(center1[0]-179)>myPrecision && fabs(center1[1]-128)>myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getPadCenter(10): -179 128!= " <<center1[0]<<" "<<center1[1]<<endl;
      failures++;
    }
  std::vector<double> center2=tpc.getModule(8,0).getPadCenter(10);
  if(fabs(center2[0]-121)>myPrecision && fabs(center2[1]-272)>myPrecision)
    {
      myfile<<"FAILED tpc.getModule(8,0).getPadCenter(10): 121 -272!= " <<center2[0]<<" "<<center2[1]<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1])-10) >myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 10!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1])<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(0)[0],tpc.getModule(0,0).getPadCenter(0)[1])) >myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 0!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(0)[0],tpc.getModule(0,0).getPadCenter(0)[1])<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(5)[0],tpc.getModule(0,0).getPadCenter(5)[1]) -5)>myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 5!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(5)[0],tpc.getModule(0,0).getPadCenter(5)[1])<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(120)[0],tpc.getModule(0,0).getPadCenter(120)[1])-120) >myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 120!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(120)[0],tpc.getModule(0,0).getPadCenter(120)[1])<<endl;
      failures++;
    }
  
  if(!failures)
    myfile<<"ALL PASSED"<<endl;
  
  myfile.close();

  return 0;
}
