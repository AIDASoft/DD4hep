
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
#include "DD4hep/Shapes.h"
#include "TGeoTube.h"
#include "TGeoMatrix.h"

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
//  cout << "-----> Inside Module (0,0):\t "<<tpc.isInsideModule(0,0,0)<<endl;
  cout << "-----> Nearest Module (0,0):\t "<<tpc.getNearestModule(0,0,0).getID()<<endl;
  cout << "TPC Module functionality:"<<endl;

  std::vector<TPCModule> mymods=tpc.getModules(0);
  cout << "-----> NModules EP0:\t " << mymods.size()<< endl;

  for(int m=0;m<mymods.size();m++)
    {
      TPCModule mod=mymods[m];
      Tube tube=mod.volume().solid();
      double x  = mod.placement()->GetMatrix()->GetTranslation()[0];
      cout<<mod.id()<<" "<<mod.getPadType()<<" "<<mod.getNPads()<<" "<<mod.getNRows()<<" "<<x<<" "<<tube->GetRmin()<<endl;
    }

  TPCModule mod=mymods[0];
 //rows
//   for(int r=0;r<mod.getNRows();r++)
//     {
//       //pads
//       for(int p=0;p<mod.getNPadsInRow(r);p++)
// 	{
// 	  std::vector<double> center=mod.getPadCenter(mod.getPadIndex(r,p));
// 	  double x=center[0];
// 	  double y=center[1];
// 	  // cout<<r<<" "<<p<<" "<<x<<","<<y <<" "<<mod.getPadIndex(r,p)<<" "<<mod.getNearestPad(x,y)<<endl;
// 	  if(mod.getPadIndex(r,p)!=mod.getNearestPad(x,y))
// 	    cout<<"FAILED: "<<mod.getPadIndex(r,p)<<" "<<mod.getNearestPad(x,y)<<endl;
// 	}
//     }
  double xstart  = mod.placement()->GetMatrix()->GetTranslation()[0];
  Tube tube=mod.volume().solid();
  for(int p=0;p<10;p++)
    {
      double y=0;
      double x=tube->GetRmin()+2+10*p;
      TPCModule mod=tpc.getNearestModule(x,y,0);
      int index=  mod.getNearestPad(x,y);
      std::vector<double> center= mod.getPadCenter(index);
      cout<<mod.id()<<" "<<x-center[0]<<" "<<y-center[1]<<endl;
    }
  return 0;
}
