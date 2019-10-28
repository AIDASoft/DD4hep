//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//
//  Simple program to dump the B-field of the world volume in a cartesian grid
// 
//  Author     : F.Gaede, DESY
//  Date       : 29 Mar 2017
//==========================================================================

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/DD4hepUnits.h"

using namespace std ;
using namespace dd4hep ;
using namespace dd4hep::detail;

//=============================================================================


static int invoke_dump_B_field(int argc, char** argv ){
  
  if( argc != 8 ) {
    std::cout << " usage: dumpBfield compact.xml x y z dx dy dz [in cm]" << std::endl 
	      << "    will dump the B-field in volume [-x:x,-y:y,-z,z] with steps [dx,dy,dz] "
	      << std::endl ;
    
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;
  
  std::stringstream sstr ;
  sstr << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] ;
  
  float xRange , yRange , zRange , dx , dy, dz ;
  sstr >> xRange >> yRange >> zRange >> dx >> dy >> dz;

  xRange *= dd4hep::cm;
  yRange *= dd4hep::cm;
  zRange *= dd4hep::cm;
  dx *= dd4hep::cm;
  dy *= dd4hep::cm;
  dz *= dd4hep::cm;

  Detector& description = Detector::getInstance();
  description.fromCompact( inFile );
  
  printf("#######################################################################################################\n");
  printf("      x[cm]            y[cm]            z[cm]          Bx[Tesla]        By[Tesla]        Bz[Tesla]     \n");

  for( float x = -xRange ; x <=xRange ; x += dx ){
    for( float y = -yRange ; y <=yRange ; y += dy ){
      for( float z = -zRange ; z <=zRange ; z += dz ){

	double posV[3] = { x, y, z }  ;
	double bfieldV[3] ;
	description.field().magneticField( posV  , bfieldV  ) ;

	printf(" %+15.8e  %+15.8e  %+15.8e  %+15.8e  %+15.8e  %+15.8e  \n",
         posV[0]/dd4hep::cm, posV[1]/dd4hep::cm,  posV[2]/dd4hep::cm,
         bfieldV[0]/dd4hep::tesla , bfieldV[1]/dd4hep::tesla, bfieldV[2]/dd4hep::tesla ) ; 

      }
    }
  }
  printf("#######################################################################################################\n");  
  return 0;
}



int main(int argc, char** argv ){
  try {
    return invoke_dump_B_field(argc,argv);
  }
  catch(const std::exception& e)  {
    std::cout << "Got uncaught exception: " << e.what() << std::endl;
  }
  catch (...)  {
    std::cout << "Got UNKNOWN uncaught exception." << std::endl;
  }
  return EINVAL;    
}

//=============================================================================
