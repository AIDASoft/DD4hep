
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

  TPCModule mod=mymods[0];
  //rows
  for(int r=0;r<mod.getNRows();r++)
    {
      //pads
      for(int p=0;p<mod.getNPadsInRow(r);p++)
	{
	  double x=mod.getPadCenter(mod.getPadIndex(r,p))[0];
	  double y=mod.getPadCenter(mod.getPadIndex(r,p))[1];
	  cout<<r<<" "<<p<<" "<<x<<","<<y <<" "<<mod.getPadIndex(r,p)<<" "<<mod.getNearestPad(x,y)<<" "<<mod.getPadIndex(r,p)-mod.getNearestPad(x,y)<<endl;
	}
    }

//   cout<<"getNPadsInRow: "<< mod.getNPadsInRow(0)<<" "<< mod.getNPadsInRow(mod.getNRows()-1)<<endl;
//   cout<<"getRowHeight: "<< mod.getRowHeight(0)<<" "<< mod.getRowHeight(mod.getNRows()-1)<<endl;
//   int padindex_min=mod.getPadIndex(0,0);
//   int padindex_max=mod.getPadIndex(mod.getNRows()-1,mod.getNPadsInRow(mod.getNRows()-1)-1);
//   cout<<"pad index :"<<padindex_min<<" =(0,0)"<<padindex_max<<" =("<<mod.getNRows()-1<<","<<mod.getNPadsInRow(mod.getNRows()-1)-1<<")"<<endl;
//   cout<<"Row nr: "<<  mod.getRowNumber(padindex_min)<<" "<< mod.getRowNumber(padindex_max)<<endl;
//   cout<<"Pad nr: "<<  mod.getPadNumber(padindex_min)<<" "<< mod.getPadNumber(padindex_max)<<endl;
//   cout<<"Pad pitch: "<<  mod.getPadPitch(padindex_min)<<" "<< mod.getPadPitch(padindex_max)<<endl;
//   cout<<"Pad center: "<<mod.getPadCenter(padindex_min)[0]<<","<<mod.getPadCenter(padindex_min)[1]<<"  "<<mod.getPadCenter(padindex_max)[0]<<","<<mod.getPadCenter(padindex_max)[1]<<endl;
//   int nearest_index1=mod.getNearestPad(mod.getPadCenter(padindex_min)[0],mod.getPadCenter(padindex_min)[1]);
//   cout<<"Nearest Pad ("<<padindex_min<<"): "<< nearest_index1<<" "<<mod.getRowNumber(nearest_index1)<<" "<<mod.getPadNumber(nearest_index1)<<endl;
//   int nearest_index2=mod.getNearestPad(mod.getPadCenter(padindex_max)[0],mod.getPadCenter(padindex_max)[1]);
//   cout<<"Nearest Pad: ("<<padindex_max<<"): "<< nearest_index2<<" "<<mod.getRowNumber(nearest_index2)<<" "<<mod.getPadNumber(nearest_index2)<<endl;
 
 
 
  return 0;
}
