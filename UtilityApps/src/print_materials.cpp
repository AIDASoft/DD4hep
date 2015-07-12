// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================
//  Simple program to print all the materials in a detector on
//  a straight line between two given points
// 
//  Author     : F.Gaede, DESY
//
//==========================================================================

// Framework include files
#include "DD4hep/LCDD.h"
#include "DD4hep/DD4hepUnits.h"
#include "DDRec/MaterialManager.h"

using namespace std ;
using namespace DD4hep ;
using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec;
using namespace DDSurfaces ;
using namespace dd4hep ;

//=============================================================================

int main(int argc, char** argv ){
    
  if( argc != 8 ) {
    std::cout << " usage: print_materials compact.xml x0 y0 z0 x1 y1 z1 " << std::endl 
              << "        -> prints the materials on a straight line between the two given points ( unit is cm) "  
              << std::endl ;
    exit(1) ;
  }
  
  std::string inFile =  argv[1] ;

  std::stringstream sstr ;
  sstr << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] ;

  double x0,y0,z0,x1,y1,z1 ;
  sstr >> x0 ;
  sstr >> y0 ;
  sstr >> z0 ;
  sstr >> x1 ;
  sstr >> y1 ;
  sstr >> z1 ;

  LCDD& lcdd = LCDD::getInstance();

  lcdd.fromCompact( inFile );

  Vector3D p0( x0, y0, z0 ) ;
  Vector3D p1( x1, y1, z1 ) ;

  MaterialManager matMgr ;

  const MaterialVec& materials = matMgr.materialsBetween( p0 , p1  ) ;
	
  std::cout  << std::endl  << " #######  materials between the two  points : " << p0 << "*cm  and " << p1 << "*cm :  "  << std::endl ;

  double sum_x0 = 0 ;
  double sum_lambda = 0 ;
  double path_length = 0 ;
  for( unsigned i=0,n=materials.size();i<n;++i){

    Material mat =  materials[i].first  ;
    double length = materials[i].second  ;

    double nx0 = length / mat.radLength()  ;
    sum_x0 += nx0 ;

    double nLambda = length / mat.intLength()  ;
    sum_lambda += nLambda ;

    path_length += length ;

    std::cout <<  "      " << mat << " thickness: " <<  length << " path_length:" << path_length  << " integrated_X0: " <<  sum_x0 << " integrated_lambda: " <<  sum_lambda << std::endl ;
  }

  std::cout << "############################################################################### "  << std::endl  << std::endl  ;
	

  const MaterialData& avMat = matMgr.createAveragedMaterial( materials ) ;

  std::cout << "     averaged Material : " << " Z: " << avMat.Z() << " A: " << avMat.A() << " densitiy: " << avMat.density()
            << " radiationLength: " <<  avMat.radiationLength() 
            << " interactionLength: " << avMat.interactionLength()  << std::endl << std::endl  ;
  

  std::cout << "     Total length : "  << path_length / dd4hep::mm << " mm "  << std::endl  ;

  std::cout << "     Integrated radiation lengths : "     << path_length / avMat.radiationLength()  << " X0 " << std::endl  ;

  std::cout << "     Integrated interaction lengths : "  << path_length / avMat.interactionLength() << " lambda "   << std::endl  ;

  std::cout << "############################################################################### "  << std::endl  << std::endl  ;


  return 0;
}

//=============================================================================
