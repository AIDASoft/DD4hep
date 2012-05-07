//
//  pymain.cpp
//  
//
//  Created by Pere Mato on 20/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "DD4hep/LCDD.h"
#include <iostream>
#include <vector>
#include "TFile.h"
#include "TGeoManager.h"
#include <string>
#include "GearTPC.h"


using namespace std;
using namespace DD4hep;
using namespace Geometry;


int main(int argc,char** argv)  {
  
  LCDD& lcdd = LCDD::getInstance();
  lcdd.fromCompact(argv[1]);

  GearTPC tpc(lcdd.detector("TPC"));
  cout << "TPC:"<<endl;
  cout << "-----> GearTPC: Inner: " << tpc.getInnerRadius() << " Outer: " << tpc.getOuterRadius() << endl;
  cout << "-----> GearTPC: DriftLength: " << tpc.getMaxDriftLength() << endl;

  //new stuff, only works with file:../DDExamples/ILDExDet/compact/ILDExTPC.xml 

  cout << "-----> GearTPC: Endplate: " << tpc.getEndPlateThickness(0) <<" "<< tpc.getEndPlateZPosition(0)<<" "<< tpc.getEndPlateZPosition(1)<< endl;
  tpc.listDetElements();
  cout << "TPC MODULES:"<<endl;
  std::vector<DDTPCModule> mymods=tpc.getModules(0);
  cout << "-----> GearTPC: NModules: " << tpc.getNModules(0) <<" "<<tpc.getNModules(1)<<" "<<mymods.size()<< endl;
  cout << "-----> GearTPC: Module 2: " << tpc.getModule(2,0)._data().id<<" "<<tpc.getModule(2,0).id()<<endl;
  DDTPCModule mymod=tpc.getModule(10,0);
  cout << "-----> GearTPC: Module 10: " <<tpc.getModule(10,0).getID()<<" "<< mymod.getID()<<endl;
  cout << "-----> GearTPC: Inside Module (500,900): "<<tpc.isInsideModule(500,900,0)<<endl;
  //  cout << "-----> GearTPC: Inside Module (-500,900): "<<tpc.isInsideModule(-500,900,0)<<endl;
//   cout << "-----> GearTPC: Inside Module (-500,-900): "<<tpc.isInsideModule(-500,-900,0)<<endl;
//   cout << "-----> GearTPC: Inside Module (500,-900): "<<tpc.isInsideModule(500,-900,0)<<endl;
//   cout << "-----> GearTPC: Inside Module (360,50): "<<tpc.isInsideModule(360,50,0)<<endl;
//   cout << "-----> GearTPC: Inside Module (400,50): "<<tpc.isInsideModule(400,50,0)<<endl;
//   cout << "-----> GearTPC: Inside Module (400,-200): "<<tpc.isInsideModule(400,-200,0)<<endl;
//   cout << "-----> GearTPC: Inside Module (0,300): "<<tpc.isInsideModule(0,300,0)<<endl;
//  cout << "-----> GearTPC: Nearest Module (-500,-900): "<<tpc.getNearestModule(-500,-900,0).getID()<<endl;
  cout << "-----> GearTPC: Nearest Module (800,0): "<<tpc.getNearestModule(800,0,0).getID()<<endl;
//   cout << "-----> GearTPC: Nearest Module (0,0): "<<tpc.getNearestModule(0,0,0).getID()<<endl;
//   cout << "-----> GearTPC: Nearest Module (-100,-200): "<<tpc.getNearestModule(-100,-200,0).getID()<<endl;
//   cout << "-----> GearTPC: Nearest Module (2000,2000): "<<tpc.getNearestModule(2000,2000,0).getID()<<endl;
/*
    cout << "TPC MODULE PADS:"<<endl;
    int tot=0;
    for(int m=0;m<tpc.getNModules(0);m++)
      {
	cout <<"-----> GearTPC: npads on module "<<m<<": "<<tpc.getModule(m,0).getNPads()
	     <<" "<<tpc.getModule(m,0).getNRows()<<" "<<tpc.getModule(m,0).getRowHeight(1)
	     <<" "<<tpc.getModule(m,0).getPadPitch(0)<<endl;
	tot+=tpc.getModule(m,0).getNPads();
      }
    cout <<"-----> GearTPC: npads in total: "<<tot<<endl;
    cout <<"-----> GearTPC: row number pad  5: "<< tpc.getModule(10,0).getRowNumber(5)<<" "<<tpc.getModule(10,0).getPadNumber(5)
	 <<" "<<tpc.getModule(10,0).getPadIndex(tpc.getModule(10,0).getRowNumber(5),tpc.getModule(10,0).getPadNumber(5))<<endl;
    cout <<"-----> GearTPC: row number pad 15: "<< tpc.getModule(10,0).getRowNumber(15)<<" "<<tpc.getModule(10,0).getPadNumber(15)
	 <<" "<<tpc.getModule(10,0).getPadIndex(tpc.getModule(10,0).getRowNumber(15),tpc.getModule(10,0).getPadNumber(15))<<endl;
    cout <<"-----> GearTPC: row number pad 58: "<< tpc.getModule(10,0).getRowNumber(58)<<" "<<tpc.getModule(10,0).getPadNumber(58)
	 <<" "<<tpc.getModule(10,0).getPadIndex(tpc.getModule(10,0).getRowNumber(58),tpc.getModule(10,0).getPadNumber(58))<<endl;
    std::vector<double> center3=tpc.getModule(5,0).getPadCenter(5);
    cout <<"-----> GearTPC: Center of mod5/pad5: "<<center3[0]<<" "<<center3[1]<<endl;
    std::vector<double> center3a=tpc.getModule(5,0).getPadCenter(20);
    cout <<"-----> GearTPC: Center of mod5/pad20: "<<center3a[0]<<" "<<center3a[1]<<endl;
    std::vector<double> center3b=tpc.getModule(5,0).getPadCenter(0);
    cout <<"-----> GearTPC: Center of mod5/pad0: "<<center3b[0]<<" "<<center3b[1]<<endl;
    std::vector<double> center4=tpc.getModule(6,0).getPadCenter(0);
    cout <<"-----> GearTPC: Center of mod6/pad0: "<<center4[0]<<" "<<center4[1]<<endl;
*/
    cout <<"Enplate POSITIV"<<endl;
    std::vector<double> center1=tpc.getModule(0,0).getPadCenter(10);
    cout <<"-----> GearTPC: Center of mod0/pad10: "<<center1[0]<<" "<<center1[1]<<endl;
    std::vector<double> center2=tpc.getModule(1,0).getPadCenter(10);
    cout <<"-----> GearTPC: Center of mod1/pad10: "<<center2[0]<<" "<<center2[1]<<endl;
    cout <<"Enplate NEGATIVE"<<endl;
    std::vector<double> center1a=tpc.getModule(0,1).getPadCenter(10);
    cout <<"-----> GearTPC: Center of mod0/pad10: "<<center1a[0]<<" "<<center1a[1]<<endl;
    std::vector<double> center2a=tpc.getModule(1,1).getPadCenter(10);
    cout <<"-----> GearTPC: Center of mod1/pad10: "<<center2a[0]<<" "<<center2a[1]<<endl;
    //cout <<"-----> GearTPC:Exception test: "<<tpc.getModule(6,0).getRowNumber(100)<<endl;

    /*    
    cout <<"Neighbours"<<endl;
    cout <<"-----> GearTPC: RightNeighbour of 0: "<<tpc.getModule(0,0).getRightNeighbour(0)<<endl;
      //	 <<" "<<tpc.getModule(0,0).getRightNeighbour(7)<<endl;
    cout <<"-----> GearTPC: LeftNeighbour of 3: "<<tpc.getModule(0,0).getLeftNeighbour(3)<<endl;
    //<<" "<<tpc.getModule(0,0).getLeftNeighbour(4)<<endl;

    cout <<"-----> GearTPC: Nearest Pad 10: "<<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1])<<endl;
    cout <<"-----> GearTPC: Nearest Pad 0: "<<tpc.getModule(5,0).getNearestPad(tpc.getModule(5,0).getPadCenter(0)[0],tpc.getModule(5,0).getPadCenter(0)[1])<<endl;
    cout <<"-----> GearTPC: Nearest Pad 5: "<<tpc.getModule(5,0).getNearestPad(tpc.getModule(5,0).getPadCenter(5)[0],tpc.getModule(5,0).getPadCenter(5)[1])<<endl;
    cout <<"-----> GearTPC: Nearest Pad 20: "<<tpc.getModule(5,0).getNearestPad(tpc.getModule(5,0).getPadCenter(20)[0],tpc.getModule(5,0).getPadCenter(20)[1])<<endl;
    cout <<"-----> GearTPC: Nearest Pad: "<<tpc.getModule(5,0).getNearestPad(591, 760)<<endl;
    */
   return 0;
}
