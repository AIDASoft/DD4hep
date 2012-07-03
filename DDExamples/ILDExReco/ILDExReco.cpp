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
  cout << "-----> Inner Radius:\t " << tpc.getInnerRadius() <<endl;
  cout << "-----> Outer Radius:\t " << tpc.getOuterRadius() << endl;
  cout << "-----> DriftLength:\t " << tpc.getMaxDriftLength() << endl;

  //new stuff, only works with file:../DDExamples/ILDExDet/compact/ILDExTPC.xml
  //tpc.listDetElements();
  cout << "-----> EndplateThickness 0:\t " << tpc.getEndPlateThickness(0) <<endl;
  cout << "-----> EndplateZPosition 0:\t "<< tpc.getEndPlateZPosition(0)<<endl;
  cout << "-----> EndplateThickness 1:\t " << tpc.getEndPlateThickness(1) <<endl;
  cout << "-----> EndplateZPosition 1:\t "<< tpc.getEndPlateZPosition(1)<<endl;
  std::vector<TPCModule> mymods=tpc.getModules(0);
  cout << "-----> NModules EP0:\t " << mymods.size()<< endl;
  cout << "-----> NModules EP1:\t " << tpc.getNModules(1) << endl;
  int npads=0;
  for(int p=0;p<mymods.size();p++)
    npads+=tpc.getModule(p,0).getNPads();
  cout << "-----> NPads EP1:\t " << npads << endl;
  cout << "-----> Module 2 EP 0 ID:\t " << tpc.getModule(2,0).id()<<endl;
  cout << "-----> Inside Module (500,900):\t "<<tpc.isInsideModule(500,900,0)<<endl;
  cout << "-----> Nearest Module (500,900) ->16:\t "<<tpc.getNearestModule(500,900,0).getID()<<endl;
  cout << "-----> Nearest Module (800,0):\t "<<tpc.getNearestModule(800,0,0).getID()<<endl;
  cout << "TPC Module functionality:"<<endl;

  TPCModule mymod=tpc.getModule(10,1);
  cout << "-----> Module 10 EP 1 ID:\t " << mymod.getID()<<endl;
  cout << "-----> Module 10 Pads:\t " << mymod.getPadType()<<endl;
  cout << "-----> Module 10 NPads:\t " << mymod.getNPads()<<endl;
  cout << "-----> Module 10 pad rows:\t " << mymod.getNRows()<<endl;
  cout << "-----> Module 10 pads in rows:\t " << mymod.getNPadsInRow(0)<<endl;
  cout << "-----> Module 10 row height:\t " << mymod.getRowHeight(0)<<endl;
  cout << "-----> Module 10 pad pitch:\t " << mymod.getPadPitch(1)<<endl;
  cout << "-----> Module 10 row number pad 15:\t " << mymod.getRowNumber(15)<<endl;
  cout << "-----> Module 10 pad number pad 15:\t " << mymod.getPadNumber(15)<<endl;
  cout << "-----> Module 10 pad index (1,5):\t " << mymod.getPadIndex(1,5)<<endl;
  cout << "-----> RightNeighbour of 0:\t "<<tpc.getModule(0,0).getRightNeighbour(0)<<endl;
  cout << "-----> LeftNeighbour of 3:\t "<<tpc.getModule(0,0).getLeftNeighbour(3)<<endl;Z
  std::vector<double> center1=tpc.getModule(0,0).getPadCenter(10);
  cout <<"-----> Center of mod0/pad10 EP0:\t "<<center1[0]<<" "<<center1[1]<<endl;
  std::vector<double> center2=tpc.getModule(1,0).getPadCenter(10);
  cout <<"-----> Center of mod1/pad10 EP0:\t "<<center2[0]<<" "<<center2[1]<<endl;
  std::vector<double> center1a=tpc.getModule(0,1).getPadCenter(10);
  cout <<"-----> Center of mod0/pad10 EP1:\t "<<center1a[0]<<" "<<center1a[1]<<endl;
  std::vector<double> center2a=tpc.getModule(1,1).getPadCenter(10);
  cout <<"-----> Center of mod1/pad10 EP1:\t "<<center2a[0]<<" "<<center2a[1]<<endl;
  cout <<"-----> Nearest Pad 10:\t "<<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1])<<endl;
  cout <<"-----> Nearest Pad 0:\t "<<tpc.getModule(5,0).getNearestPad(tpc.getModule(5,0).getPadCenter(0)[0],tpc.getModule(5,0).getPadCenter(0)[1])<<endl;
  cout <<"-----> Nearest Pad 5:\t "<<tpc.getModule(5,0).getNearestPad(tpc.getModule(5,0).getPadCenter(5)[0],tpc.getModule(5,0).getPadCenter(5)[1])<<endl;
#if 0
  cout <<"-----> Nearest Pad 20:\t "<<tpc.getModule(5,0).getNearestPad(tpc.getModule(5,0).getPadCenter(20)[0],tpc.getModule(5,0).getPadCenter(20)[1])<<endl;
  cout <<"-----> Nearest Pad 591, 760:\t "<<tpc.getModule(5,0).getNearestPad(591, 760)<<endl;
#endif
  cout << "Ctor:" << (void*)&typeid(TPCModuleData) << endl;
  for(int i=0; i<10; ++i) {
    for(int j=0; j<2; ++j) {
      typedef Value<Value<TNamed,DetElement::Object>,TPCModuleData> Val;
      DetElement elt = tpc.getModule(i,j);
      TPCModule mod = elt;//tpc.getModule(i,j);
      if ( mod.isValid() ) {
	TPCModuleData* d = mod.m_modData;
	cout << mod.name() << " parent:" << mod.parent().name() << " padHeight:" << d->padHeight << " " << endl;
      }
    }
  }
  try{
    int row=tpc.getModule(6,0).getRowNumber(-10);
  }
  catch(OutsideGeometryException e){cout<<"-----> Exception test: "<<e.what()<<endl;};

  cout<<"Test Data Block extension:"<<endl;
  //works for endplate 0, but not for the deep copy of endplate 1
  TPCModule mymod2=tpc.getModule(10,0);
  TPCModuleData* tpcModData= (TPCModuleData*)&mymod2._data();
  cout<<tpcModData->padGap <<endl;

 //  for(int i=0; i<10;++i) {
//     for (int j=0; j<2;++j) {
//       TPCModule m=tpc.getModule(i,j);
//       TPCModuleData* tpcModData= (TPCModuleData*)&m._data();
//       cout<<tpcModData->padGap <<endl;
//     }}
  return 0;
}
