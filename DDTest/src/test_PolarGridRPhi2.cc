#include "DDSegmentation/PolarGridRPhi2.h"
#include "DD4hep/DDTest.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <exception>


static dd4hep::DDTest test( "PolarGridRPhi2" ) ;

class TestTuple {
public:
  double    _r;
  double    _p; //phi
  long long _rB;
  long long _pB;
  TestTuple( double r, double p, long long rB, long long pB): _r(r), _p(p), _rB(rB), _pB(pB) {}
};

int main() {
  try{

    dd4hep::DDSegmentation::PolarGridRPhi2 seg("system:8,layer:8,barrel:3,layer:8,slice:5,r:16,phi:16");
    const int rBitOffset   = 32; //sum of all the others before
    const int phiBitOffset = 16+32; //sum of all the others before

    std::vector<double> rValues, phiValues;

    rValues.push_back( 10);//  0
    rValues.push_back( 20);//  1
    rValues.push_back( 30);//  2
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
    rValues.push_back(160);// 15

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

    seg.setGridRValues(rValues);
    seg.setGridPhiValues(phiValues);
    seg.setOffsetPhi(-M_PI);

    std::vector<TestTuple> tests;
    tests.push_back( TestTuple( 10.0, -180*DegToRad,   0,   0 ) );
    tests.push_back( TestTuple( 10.1,  175*DegToRad,   0,  35 ) );
    tests.push_back( TestTuple( 20.0,   22*DegToRad,   1,  10 ) );
    tests.push_back( TestTuple( 20.1,   40*DegToRad,   1,  11 ) );
    tests.push_back( TestTuple( 15.0,  359*DegToRad,   0,  17 ) );
    tests.push_back( TestTuple( 42.0,   42*DegToRad,   3,   5 ) );
    tests.push_back( TestTuple( 22.0,    0*DegToRad,   1,   9 ) );
    tests.push_back( TestTuple( 22.0,  180*DegToRad,   1,  18 ) );
    tests.push_back( TestTuple( 22.0,  -180*DegToRad,   1,  0 ) );
    tests.push_back( TestTuple(  10.1, -179*DegToRad,     0,   0 ) );
    tests.push_back( TestTuple(  20.1, -179*DegToRad,     1,   0 ) );
    tests.push_back( TestTuple(  30.1, -179*DegToRad,     2,   0 ) );
    tests.push_back( TestTuple(  40.1, -179*DegToRad,     3,   0 ) );
    tests.push_back( TestTuple(  50.1, -179*DegToRad,     4,   0 ) );
    tests.push_back( TestTuple(  60.1, -179*DegToRad,     5,   0 ) );
    tests.push_back( TestTuple(  70.1, -179*DegToRad,     6,   0 ) );
    tests.push_back( TestTuple(  80.1, -179*DegToRad,     7,   0 ) );
    tests.push_back( TestTuple(  90.1, -179*DegToRad,     8,   0 ) );
    tests.push_back( TestTuple( 100.1, -179*DegToRad,     9,   0 ) );
    tests.push_back( TestTuple( 110.1, -179*DegToRad,    10,   0 ) );
    tests.push_back( TestTuple( 120.1, -179*DegToRad,    11,   0 ) );
    tests.push_back( TestTuple( 130.1, -179*DegToRad,    12,   0 ) );
    tests.push_back( TestTuple( 140.1, -179*DegToRad,    13,   0 ) );
    tests.push_back( TestTuple( 150.1, -179*DegToRad,    14,   0 ) );
    tests.push_back( TestTuple( 160.0, -179*DegToRad,    14,   0 ) );


    dd4hep::DDSegmentation::VolumeID volID = 0;

    //Test from position to cellID
    for(std::vector<TestTuple>::const_iterator it = tests.begin(); it != tests.end(); ++it) {

      const double r     = (*it)._r;
      const double phi   = (*it)._p;
      const long long rB = (*it)._rB;
      const long long pB = (*it)._pB;

      dd4hep::DDSegmentation::Vector3D locPos ( r*cos(phi), r*sin(phi), 0.0);
      dd4hep::DDSegmentation::Vector3D globPos( r*cos(phi), r*sin(phi), 0.0);

      dd4hep::DDSegmentation::CellID cid = seg.cellID(locPos, globPos, volID);

      const long long phiShifted(pB << phiBitOffset);
      const long long rShifted  (rB << rBitOffset);
      const long long expectedID(rShifted + phiShifted);

      test( expectedID , cid , " Test get ID From Position" );

      std::cout << std::setw(20) <<  " "
		<< std::setw(20) <<  "rBin     "
		<< std::setw(20) <<  "pBin     "
		<< std::endl;

      std::cout << std::setw(20) <<  "Expected"
		<< std::setw(20) <<  rB
		<< std::setw(20) <<  pB
		<< std::endl;

      std::cout << std::setw(20) <<  "Calculated"
		<< std::setw(20) <<  (*seg.decoder())["r"]
		<< std::setw(20) <<  (*seg.decoder())["phi"]
		<< std::endl;

    }



    std::vector<TestTuple> testPositions;
    testPositions.push_back( TestTuple( 15.0, -175*DegToRad,   0,   0 ) );
    testPositions.push_back( TestTuple( 15.0,  175*DegToRad,   0,  35 ) );
    testPositions.push_back( TestTuple( 25.0, -150*DegToRad,   1,  1 ) );
    testPositions.push_back( TestTuple( 35.0, -135*DegToRad,   2,  1 ) );
    testPositions.push_back( TestTuple( 45.0, -120*DegToRad,   3,  1 ) );
    testPositions.push_back( TestTuple( 55.0, -105*DegToRad,   4,  1 ) );
    testPositions.push_back( TestTuple( 65.0,  -90*DegToRad,   5,  1 ) );
    testPositions.push_back( TestTuple( 75.0,  -75*DegToRad,   6,  1 ) );
    testPositions.push_back( TestTuple( 85.0,  -60*DegToRad,   7,  1 ) );
    testPositions.push_back( TestTuple( 95.0,  -45*DegToRad,   8,  1 ) );
    testPositions.push_back( TestTuple( 105.0, -30*DegToRad,   9,  1 ) );
    testPositions.push_back( TestTuple( 115.0, -15*DegToRad,  10,  1 ) );
    testPositions.push_back( TestTuple( 125.0,  -0*DegToRad,  11,  1 ) );
    testPositions.push_back( TestTuple( 135.0,  15*DegToRad,  12,  1 ) );
    testPositions.push_back( TestTuple( 145.0,  30*DegToRad,  13,  1 ) );
    testPositions.push_back( TestTuple( 155.0,  45*DegToRad,  14,  1 ) );
    testPositions.push_back( TestTuple(155.0, -105*DegToRad,  14,   0 ) );

    //Test from cellID to position
    for(std::vector<TestTuple>::const_iterator it = testPositions.begin(); it != testPositions.end(); ++it) {

      const double r     = (*it)._r;
      const double phi   = (*it)._p;
      const long long rB = (*it)._rB;
      const long long pB = (*it)._pB;

      (*seg.decoder())["r"] = rB;
      (*seg.decoder())["phi"] = pB;

      dd4hep::DDSegmentation::CellID cellID = (*seg.decoder()).getValue();
      std::cout << "CellID: " << cellID  << std::endl;

      dd4hep::DDSegmentation::Vector3D expectedPosition( r*cos(phi), r*sin(phi), 0.0);
      dd4hep::DDSegmentation::Vector3D calculatedPosition = seg.position(cellID);

      test( fabs(expectedPosition.x() - calculatedPosition.x())  < 1e-11, " Test get Position from ID: X" );
      test( fabs(expectedPosition.y() - calculatedPosition.y())  < 1e-11, " Test get Position from ID: Y" );
      test( fabs(expectedPosition.z() - calculatedPosition.z())  < 1e-11, " Test get Position from ID: Z" );

      std::cout << std::setw(20) <<  " "
		<< std::setw(20) <<  "r     "
		<< std::setw(20) <<  "phi     "
		<< std::endl;

      std::cout << std::setw(20) <<  "Expected"
		<< std::setw(20) <<  r
		<< std::setw(20) <<  phi/DegToRad
		<< std::endl;

      const double rCalc =
	sqrt( calculatedPosition.x() * calculatedPosition.x() +
	      calculatedPosition.y() * calculatedPosition.y() );
      const double pCalc = atan2( calculatedPosition.y(), calculatedPosition.x() );

      std::cout << std::setw(20) <<  "Calculated"
		<< std::setw(20) <<  rCalc
		<< std::setw(20) <<  pCalc/DegToRad
		<< std::endl;

    }



  } catch( std::exception &e ){
    //} catch( ... ){

    test.log( e.what() );
    test.error( "exception occurred" );
  }
  return 0;

}
