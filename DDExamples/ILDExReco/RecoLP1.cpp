
//====================================================================
//  Test application for LP1 functionality
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

//   int npads=0;
//   for(int p=0;p<mymods.size();p++)
//     npads+=tpc.getModule(p,0).getNPads();
//  cout << "-----> NPads EP0:\t " << npads << endl;
  cout << "-----> Inside Module (0,0):\t "<<tpc.isInsideModule(0,0,0)<<endl;
  cout << "-----> Nearest Module (0,0):\t "<<tpc.getNearestModule(0,0,0).getID()<<endl;
  cout << "TPC Module functionality:"<<endl;

  std::vector<TPCModule> mymods=tpc.getModules(0);
  cout << "-----> NModules EP0:\t " << mymods.size()<< endl;

  for(int m=0;m<mymods.size();m++)
    {
      TPCModule mod=mymods[m];
      cout<<mod.id()<<" "<<mod.getPadType()<<" "<<mod.getNPads()<<" "<<mod.getNRows()<<endl;
    }

  return 0;
}
