#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/PolarGridRPhi2.h"
#include "DDSegmentation/PolarGridRPhi.h"
#include "DD4hep/DDTest.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <exception>
#include <cmath>

static DD4hep::DDTest test( "CellDimensions" ) ;

using DD4hep::DDSegmentation::Segmentation;
using DD4hep::DDSegmentation::CellID;

Segmentation* createPolarGridRPhi2();
Segmentation* createPolarGridRPhi(double rSize, double phiSize);
CellID getCellID(Segmentation* seg, long long rId, long long pId);

class TestTuple {
public:
  double    _r;
  double    _p; ///phi
  CellID _cid; ///cellID
  TestTuple( Segmentation* seg, double r, double p, long long rB, long long rP): _r(r), _p(p), _cid(getCellID(seg, rB, rP)) {}
};

void testRPhi2();
void testRPhi();

int main() {

  testRPhi2();
  testRPhi();

  return 0;
}


Segmentation* createPolarGridRPhi(double rSize, double phiSize) {

  DD4hep::DDSegmentation::PolarGridRPhi* seg = new DD4hep::DDSegmentation::PolarGridRPhi("system:8,barrel:3,layer:8,slice:13,r:16,phi:16");

  seg->setGridSizeR(rSize);
  seg->setGridSizePhi(phiSize);
  return seg;

}

Segmentation* createPolarGridRPhi2() {

  DD4hep::DDSegmentation::PolarGridRPhi2* seg = new DD4hep::DDSegmentation::PolarGridRPhi2("system:8,barrel:3,layer:8,slice:13,r:16,phi:16");

  std::vector<double> rValues, phiValues;

  rValues.push_back( 10);//  0
  rValues.push_back( 30);//  1
  rValues.push_back( 35);//  2
  rValues.push_back( 40);//  3
  rValues.push_back( 50);//  4
  rValues.push_back( 60);//  5
  rValues.push_back( 70);//  6
  rValues.push_back( 80);//  7
  rValues.push_back( 90);//  8
  rValues.push_back(100);//  9
  rValues.push_back(110);// 10
  rValues.push_back(120);// 11
  rValues.push_back(130);// 12
  rValues.push_back(140);// 13
  rValues.push_back(150);// 14
  rValues.push_back(153);// 15

  const double DegToRad(M_PI/180.0);

  phiValues.push_back( 10*DegToRad);//  0
  phiValues.push_back( 20*DegToRad);//  1
  phiValues.push_back( 30*DegToRad);//  2
  phiValues.push_back( 40*DegToRad);//  3
  phiValues.push_back( 50*DegToRad);//  4
  phiValues.push_back( 60*DegToRad);//  5
  phiValues.push_back( 70*DegToRad);//  6
  phiValues.push_back( 80*DegToRad);//  7
  phiValues.push_back( 90*DegToRad);//  8
  phiValues.push_back(100*DegToRad);//  9
  phiValues.push_back(110*DegToRad);// 10
  phiValues.push_back(120*DegToRad);// 11
  phiValues.push_back(130*DegToRad);// 12
  phiValues.push_back(140*DegToRad);// 13
  phiValues.push_back(150*DegToRad);// 14
  //need one less phiValue than radial segments
  //phiValues.push_back(160*DegToRad);// 15

  seg->setGridRValues(rValues);
  seg->setGridPhiValues(phiValues);
  seg->setOffsetPhi(-M_PI);

  return seg;
}

CellID getCellID(DD4hep::DDSegmentation::Segmentation* seg, long long rB, long long pB){
  (*seg->decoder())["r"] = rB;
  (*seg->decoder())["phi"] = pB;
  return (*seg->decoder()).getValue();
}

void testRPhi2(){
  std::vector<TestTuple> tests;

  DD4hep::DDSegmentation::Segmentation* seg = createPolarGridRPhi2();
  const double DegToRad = M_PI/180.0;
  tests.push_back( TestTuple( seg, 20.0, 20*10*DegToRad,  0,   1 ) );
  tests.push_back( TestTuple( seg, 5.0, 32.5*20*DegToRad,  1,   1 ) );
  tests.push_back( TestTuple( seg, 3.0, 151.5*150*DegToRad,  14, 1 ) );

  try {

    for (std::vector<TestTuple>::iterator it = tests.begin(); it != tests.end(); ++it) {

      CellID cellid   = (*it)._cid;
      double rSize    = (*it)._r;
      double rPhiSize = (*it)._p;

      test( fabs(seg->cellDimensions(cellid)[0] - rSize )  < 1e-11, " Seg: RPhi2: Dimension for R" );
      test( fabs(seg->cellDimensions(cellid)[1] - rPhiSize )  < 1e-11, " Seg: RPhi2: Dimension for RPhi" );

    }
  } catch( std::exception &e ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

}

void testRPhi(){
  std::vector<TestTuple> tests;

  const double rSizeGrid = 10.0;
  const double phiSizeGrid = M_PI/36;
  DD4hep::DDSegmentation::Segmentation* seg = createPolarGridRPhi(rSizeGrid, phiSizeGrid);

  tests.push_back( TestTuple( seg, rSizeGrid, rSizeGrid*(0) *phiSizeGrid,   0, 1 ) );
  tests.push_back( TestTuple( seg, rSizeGrid, rSizeGrid*(1) *phiSizeGrid,   1, 1 ) );
  tests.push_back( TestTuple( seg, rSizeGrid, rSizeGrid*(14)*phiSizeGrid,  14, 1 ) );

  try {

    for (std::vector<TestTuple>::iterator it = tests.begin(); it != tests.end(); ++it) {

      CellID cellid   = (*it)._cid;
      double rSize    = (*it)._r;
      double rPhiSize = (*it)._p;

      test( fabs(seg->cellDimensions(cellid)[0] - rSize )  < 1e-11, " Seg: RPhi: Dimension for R" );
      test( fabs(seg->cellDimensions(cellid)[1] - rPhiSize )  < 1e-11, " Seg: RPhi: Dimension for RPhi" );

    }
  } catch( std::exception &e ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }

}
