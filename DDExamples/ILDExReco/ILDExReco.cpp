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

  GearTPC tpc = lcdd.detector("TPC");

  cout << "-----> Gear: Inner:" << tpc.innerRadius() << " Outer:" << tpc.outerRadius() << endl;
  cout << "-----> Gear: Press:" << tpc.pressure() << endl;
  //cout << "-----> Gear: NModukes:" << tpc.getNModules() << endl;
  //cout << "-----> Gear: DriftLength:" << tpc.getDriftLength() << endl;
  
  
  return 0;
}
