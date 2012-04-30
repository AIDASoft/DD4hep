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

  cout << "-----> GearTPC: Inner: " << tpc.getInnerRadius() << " Outer: " << tpc.getOuterRadius() << endl;
  cout << "-----> GearTPC: DriftLength: " << tpc.getMaxDriftLength() << endl;

  //new stuff, only works with file:../DDExamples/ILDExDet/compact/ILDExTPC.xml 

  cout << "-----> GearTPC: Endplate: " << tpc.getEndPlateThickness() << endl;
  cout << "-----> GearTPC: NModules: " << tpc.getNModules(0) <<" "<<tpc.getNModules(1)<< endl;
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
  cout << "-----> GearTPC: Nearest Module (-500,-900): "<<tpc.getNearestModule(-500,-900,0).getID()<<endl;
  cout << "-----> GearTPC: Nearest Module (800,0): "<<tpc.getNearestModule(800,0,0).getID()<<endl;
  cout << "-----> GearTPC: Nearest Module (0,0): "<<tpc.getNearestModule(0,0,0).getID()<<endl;
  cout << "-----> GearTPC: Nearest Module (-100,-200): "<<tpc.getNearestModule(-100,-200,0).getID()<<endl;
  cout << "-----> GearTPC: Nearest Module (2000,2000): "<<tpc.getNearestModule(2000,2000,0).getID()<<endl;

  
  return 0;
}
