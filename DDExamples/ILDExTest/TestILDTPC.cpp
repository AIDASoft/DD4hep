
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
  myfile.open ("testOutput_TestILCTPC.txt");

  //based on the reference value precision input
  double myPrecision=1e-3;
  int failures=0;
  GearTPC tpc(lcdd.detector("TPC"));
  if(fabs(tpc.getInnerRadius() - 350) > myPrecision )
    {
      myfile << "FAILED tpc.getInnerRadius(): 350!=" <<tpc.getInnerRadius()<<endl;
      failures++;
    }
  if(fabs( tpc.getOuterRadius() - 1800 )> myPrecision )
    {
      myfile << "FAILED tpc.getOuterRadius(): 1800!="<<tpc.getOuterRadius()<< endl;
      failures++;
    }
  if( fabs( tpc.getMaxDriftLength() -2496.5 )> myPrecision)
    {
      myfile << "FAILED tpc.getMaxDriftLength(): 2496.5!=" << tpc.getMaxDriftLength()<<endl;
      failures++;
    }
  if(fabs(tpc.getEndPlateThickness(0) - 1.5) > myPrecision)
    {
      myfile << "FAILED tpc.getEndPlateThickness(0): 1.5!=" <<tpc.getEndPlateThickness(0)<<endl;
      failures++;
    }
  if(fabs(tpc.getEndPlateZPosition(0)-2498.5)> myPrecision)
    {
      myfile << "FAILED tpc.getEndPlateZPosition(0): 2498.5!="<<tpc.getEndPlateZPosition(0)<<endl;
      failures++;
    }
  if(fabs(tpc.getEndPlateThickness(1)-1.5)> myPrecision)
    {
      myfile << "FAILED tpc.getEndPlateThickness(1): 1.5!=" <<tpc.getEndPlateThickness(1)<<endl;
      failures++;
    }
  if(fabs(tpc.getEndPlateZPosition(1)-(-2498.5)) > myPrecision)
    {
      myfile << "FAILED tpc.getEndPlateZPosition(1): -2498.5!="<<tpc.getEndPlateZPosition(1)<<endl;
      failures++;
    }
  std::vector<TPCModule> mymods=tpc.getModules(0);
  if(fabs(mymods.size()-63)> myPrecision)
    {
      myfile << "FAILED mymods.size(): 63!="<<mymods.size()<< endl;
      failures++;
    }
  if(fabs(tpc.getNModules(1)-63)> myPrecision)
    {
      myfile << "FAILED tpc.getNModules(1): 63!= "<<tpc.getNModules(1)<< endl;
      failures++;
    }
  int npads=0;
  for(int p=0;p<mymods.size();p++)
    npads+=tpc.getModule(p,0).getNPads();
  if(fabs(npads-918456)> myPrecision)
    {
      myfile << "FAILED npads: 918456!=" <<npads<< endl;
      failures++;
    }
  if( fabs(tpc.getModule(2,0).id()-2)> myPrecision)
    {
      myfile << "FAILED tpc.getModule(2,0).id(): 2!="<< tpc.getModule(2,0).id()<<endl;
      failures++;
    }
  if(fabs(tpc.isInsideModule(500,900,0)-1)> myPrecision)
    {
      myfile << "FAILED tpc.isInsideModule(500,900,0): 1!="<<tpc.isInsideModule(500,900,0)<<endl;
      failures++;
    }
  if(fabs(tpc.getNearestModule(500,900,0).getID()-16)> myPrecision)
    {
      myfile << "FAILED tpc.getNearestModule(500,900,0).getID(): 16!="<<tpc.getNearestModule(500,900,0).getID()<<endl;
      failures++;
    }
  if(fabs(tpc.getNearestModule(800,0,0).getID()-8)> myPrecision)
    {
      myfile << "FAILED tpc.getNearestModule(800,0,0).getID(): 8!="<<tpc.getNearestModule(800,0,0).getID()<<endl;
      failures++;
    }

  TPCModule mymod=tpc.getModule(10,1);
  if(fabs(mymod.getID()-10)> myPrecision)
    {
      myfile << "FAILED mymod.getID(): 10!="<<mymod.getID()<<endl;
      failures++;
    }
  if(mymod.getPadType()!="projective_cylinder")
    {
      myfile << "FAILED mymod.getPadType(): projective_cylinder!="<<mymod.getPadType()<<endl;
      failures++;
    }
  if(fabs(mymod.getNPads()-14007)> myPrecision)
    {
      myfile << "FAILED mymod.getNPads(): 14007!="<<mymod.getNPads()<<endl;
      failures++;
    }
  if(fabs(mymod.getNRows()-21)> myPrecision)
    {
      myfile << "FAILED mymod.getNRows(): 21!="<<mymod.getNRows()<<endl;
      failures++;
    }
  if(fabs(mymod.getNPadsInRow(0)-667)> myPrecision)
    {
      myfile << "FAILED mymod.getNPadsInRow(0): 667!="<<mymod.getNPadsInRow(0)<<endl;
      failures++;
    }
  if(fabs( mymod.getRowHeight(0)-10)> myPrecision)
    {
      myfile << "FAILED mymod.getRowHeight(0): 10!="<<mymod.getRowHeight(0)<<endl;
      failures++;
    }
  if(fabs(mymod.getPadPitch(1)-0.652499)> myPrecision)
    {
      myfile << "FAILED mymod.getPadPitch(1): 0.652499!="<<mymod.getPadPitch(1)<<endl;
      failures++;
    }
  //padindex should be obtained by using getPadIndex(row,pad) not assumed by hand like here in this example
  if(fabs(mymod.getRowNumber(15)-0)> myPrecision)
    {
      myfile << "FAILED mymod.getRowNumber(15): 0!="<<mymod.getRowNumber(15)<<endl;
      failures++;
    }
  if( fabs(mymod.getPadNumber(15)-15)> myPrecision)
    {
      myfile << "FAILED mymod.getPadNumber(15): 15!="<<mymod.getPadNumber(15)<<endl;
      failures++;
    }
  if( fabs(mymod.getPadIndex(1,5)-672)> myPrecision)
    {
      myfile << "FAILED mymod.getPadIndex(1,5): 672!="<<mymod.getPadIndex(1,5)<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getRightNeighbour(0)-1)> myPrecision)
    {
      myfile << "FAILED tpc.getModule(0,0).getRightNeighbour(0): 1!="<<tpc.getModule(0,0).getRightNeighbour(0)<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getLeftNeighbour(3)-2)> myPrecision)
    {
      myfile << "FAILED tpc.getModule(0,0).getLeftNeighbour(3): 2!="<<tpc.getModule(0,0).getLeftNeighbour(3)<<endl;
      failures++;
    }
  
  std::vector<double> center1=tpc.getModule(0,0).getPadCenter(10);
  if(fabs(center1[0]-406.515) > myPrecision || fabs(center1[1]-19.8558) > myPrecision)
    {
      myfile <<"FAILED  tpc.getModule(0,0).getPadCenter(10)  406.515 19.8558 != "<<center1[0]<<" "<<center1[1]<<endl;
      failures++;
    }
  std::vector<double> center2=tpc.getModule(1,0).getPadCenter(10);
  if(fabs(center2[0]-106.736)> myPrecision || fabs(center2[1]-392.755)> myPrecision)
    {
      myfile <<"FAILED tpc.getModule(1,0).getPadCenter(10) 106.736 392.755 != "<<center2[0]<<" "<<center2[1]<<endl;
      failures++;
    }
  std::vector<double> center1a=tpc.getModule(0,1).getPadCenter(10);
  if(fabs(center1a[0]-(-406.515)) > myPrecision || fabs(center1a[1]-19.8558) > myPrecision)
    {
      myfile <<"FAILED tpc.getModule(0,1).getPadCenter(10) -406.515 19.8558 != "<<center1a[0]<<" "<<center1a[1]<<endl;
      failures++;
    }
  std::vector<double> center2a=tpc.getModule(1,1).getPadCenter(10);
  if(fabs(center2a[0]-(-106.736))> myPrecision || fabs(center2a[1]-392.755) > myPrecision)
    {
      myfile <<"FAILED tpc.getModule(1,1).getPadCenter(10) -106.736 392.755 != "<<center2a[0]<<" "<<center2a[1]<<endl;
      failures++;
    }

  if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1]) -10)> myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 10!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(10)[0],tpc.getModule(0,0).getPadCenter(10)[1])<<endl;
      failures++;
    }
  if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(0)[0],tpc.getModule(0,0).getPadCenter(0)[1]) -0)> myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 0!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(0)[0],tpc.getModule(0,0).getPadCenter(0)[1])<<endl;
      failures++;
    }
if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(5)[0],tpc.getModule(0,0).getPadCenter(5)[1]) -5)> myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 5!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(5)[0],tpc.getModule(0,0).getPadCenter(5)[1])<<endl;
      failures++;
    }
 if(fabs(tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(120)[0],tpc.getModule(0,0).getPadCenter(120)[1]) -120)> myPrecision)
    {
      myfile<<"FAILED tpc.getModule(0,0).getNearestPad(): 120!=" <<tpc.getModule(0,0).getNearestPad(tpc.getModule(0,0).getPadCenter(120)[0],tpc.getModule(0,0).getPadCenter(120)[1])<<endl;
    }

  if(!failures)
    myfile<<"ALL PASSED"<<endl;

  myfile.close();
  
  return 0;
}
