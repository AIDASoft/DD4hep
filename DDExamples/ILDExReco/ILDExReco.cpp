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
  cout << "-----> GearTPC: Endplate: " << tpc.getEndPlateThickness() << endl;
  cout << "-----> GearTPC: NModules: " << tpc.getNModules(0) <<" "<<tpc.getNModules(1)<< endl;
  cout << "-----> GearTPC: Module 2: " << tpc.getModule(2,0)._data().id<<" "<<tpc.getModule(2,0).id()<<endl;
  cout << "-----> GearTPC: Module 60: " << tpc.getModule(60,0)._data().id<<endl;
  DDTPCModule mymod=tpc.getModule(60,0);
  cout << "-----> GearTPC: Module 60: " << mymod.getID()<<endl;
//   cout << "-----> Gear: Children::"<< endl;
//     tpc.listDetElements();
  
  return 0;
}
