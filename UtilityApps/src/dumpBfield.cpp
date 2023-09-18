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
    std::cout << " usage: dumpBfield compact.xml xmin:xmax ymin:ymax zmin:zmax dx dy dz [in cm]" << std::endl 
	      << "    will dump the B-field in volume [xmin:xmax, ymin:ymax, zmin:zmax] with steps [dx,dy,dz] "
	      << std::endl ;
    
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;
  
  std::stringstream sstr ;
  sstr << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] ;
  
  std::string RangeX , RangeY , RangeZ;
  float dx , dy, dz ;

  sstr >> RangeX >> RangeY >> RangeZ >> dx >> dy >> dz;

  size_t colon_posX = RangeX.find(':');
  size_t colon_posY = RangeY.find(':');
  size_t colon_posZ = RangeZ.find(':');

  float minX=0, maxX=0, minY=0, maxY=0, minZ=0, maxZ=0;

  if( colon_posX == std::string::npos || colon_posY == std::string::npos || colon_posZ == std::string::npos ) {
    // symmetric intervals
    std::cout << "Intervals not specified as xmin:xmax ymin:ymax zmin:zmax" << std::endl
              << "  setting xmin = -xmax, ymin = -ymax, zmin = -zmax " << std::endl;
    maxX = std::stof( RangeX );
    maxY = std::stof( RangeY );
    maxZ = std::stof( RangeZ );
    minX = -maxX;
    minY = -maxY;
    minZ = -maxZ;
  }
  else { // asymmetric intervals
    minX = std::stof( RangeX.substr(0, colon_posX) );
    maxX = std::stof( RangeX.substr(colon_posX+1) );
    minY = std::stof( RangeY.substr(0, colon_posY) );
    maxY = std::stof( RangeY.substr(colon_posY+1) );
    minZ = std::stof( RangeZ.substr(0, colon_posZ) );
    maxZ = std::stof( RangeZ.substr(colon_posZ+1) );
  }

  minX *= dd4hep::cm;
  maxX *= dd4hep::cm;
  minY *= dd4hep::cm;
  maxY *= dd4hep::cm;
  minZ *= dd4hep::cm;
  maxZ *= dd4hep::cm;
  dx *= dd4hep::cm;
  dy *= dd4hep::cm;
  dz *= dd4hep::cm;

  Detector& description = Detector::getInstance();
  description.fromCompact( inFile );
  
  printf("#######################################################################################################\n");
  printf("      x[cm]            y[cm]            z[cm]          Bx[Tesla]        By[Tesla]        Bz[Tesla]     \n");

  for( float x = minX ; x <= maxX ; x += dx ){
    for( float y = minY ; y <= maxY ; y += dy ){
      for( float z = minZ ; z <= maxZ ; z += dz ){

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
