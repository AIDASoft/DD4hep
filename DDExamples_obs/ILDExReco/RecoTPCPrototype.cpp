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

  GearTPC tpc(lcdd.detector("TPC"));
  cout << "Gear TPC functionality:"<<endl;
  cout << "-----> Outer Radius:\t " << tpc.getOuterRadius() << endl;
  cout << "-----> DriftLength:\t " << tpc.getMaxDriftLength() << endl;

  cout << "-----> EndplateThickness 0:\t " << tpc.getEndPlateThickness(0) <<endl;
  cout << "-----> EndplateZPosition 0:\t "<< tpc.getEndPlateZPosition(0)<<endl;
  try{
    cout << "-----> EndplateThickness 1:\t " << tpc.getEndPlateThickness(1) <<endl;
    cout << "-----> EndplateZPosition 1:\t "<< tpc.getEndPlateZPosition(1)<<endl;
  }
  catch(OutsideGeometryException e){cout<<"-----> Exception test: "<<e.what()<<endl;};

  std::vector<TPCModule> mymods=tpc.getModules(0);
  cout << "-----> NModules EP0:\t " << mymods.size()<< endl;
  int npads=0;
  for(int p=0;p<mymods.size();p++)
    npads+=tpc.getModule(p,0).getNPads();
  cout << "-----> NPads EP0:\t " << npads << endl;
  cout << "-----> Inside Module (-150,0):\t "<<tpc.isInsideModule(-150,0,0)<<endl;
  cout << "-----> Nearest Module (-150,0):\t "<<tpc.getNearestModule(-150,0,0).getID()<<endl;
  cout << "-----> Nearest Module (800,0):\t "<<tpc.getNearestModule(800,0,0).getID()<<endl;
  cout << "TPC Module functionality:"<<endl;

  TPCModule mymod=tpc.getModule(4,0);
  cout << "-----> Module 4 EP 0 ID:\t " << mymod.getID()<<endl;
  cout << "-----> Module 4 Pads:\t " << mymod.getPadType()<<endl;
  cout << "-----> Module 4 NPads:\t " << mymod.getNPads()<<endl;
  cout << "-----> Module 4 pad rows:\t " << mymod.getNRows()<<endl;
  cout << "-----> Module 4 pads in row:\t " << mymod.getNPadsInRow(0)<<endl;
  cout << "-----> Module 4 row height:\t " << mymod.getRowHeight(0)<<endl;
  cout << "-----> Module 4 pad pitch:\t " << mymod.getPadPitch(1)<<endl;
  cout << "-----> Module 4 row number pad 15:\t " << mymod.getRowNumber(15)<<endl;
  cout << "-----> Module 4 pad number pad 15:\t " << mymod.getPadNumber(15)<<endl;
  cout << "-----> Module 4 pad index (1,5):\t " << mymod.getPadIndex(1,5)<<endl;
  cout << "-----> RightNeighbour of 0:\t "<<tpc.getModule(0,0).getRightNeighbour(0)<<endl;
  cout << "-----> LeftNeighbour of 3:\t "<<tpc.getModule(0,0).getLeftNeighbour(3)<<endl;
  std::vector<double> center1=tpc.getModule(0,0).getPadCenter(10);
  cout <<"-----> Center of mod0/pad10 EP0:\t "<<center1[0]<<" "<<center1[1]<<endl;
  std::vector<double> center2=tpc.getModule(8,0).getPadCenter(10);
  cout <<"-----> Center of mod9/pad10 EP0:\t "<<center2[0]<<" "<<center2[1]<<endl;
  cout <<"-----> Nearest Pad 10:\t "<<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1])<<endl;
  cout <<"-----> Nearest Pad 0:\t "<<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(0)[0],tpc.getModule(0,0).getPadCenter(0)[1])<<endl;
  cout <<"-----> Nearest Pad 5:\t "<<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(5)[0],tpc.getModule(0,0).getPadCenter(5)[1])<<endl;
  cout <<"-----> Nearest Pad 120:\t "<<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(120)[0],tpc.getModule(0,0).getPadCenter(120)[1])<<endl;


  return 0;
}
