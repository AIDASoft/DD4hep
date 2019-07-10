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
//  Compute the  material budget in terms of integrated radiation and ineraction
//  lengths as seen from the IP for various subdetector regions given in
//  a simple steering file. Creates a root file w/ histograms and dumps numbers
//  the screen.
// 
//  Author     : F.Gaede, DESY
//
//==========================================================================

#include "TError.h"

// Framework include files
#include "DD4hep/Detector.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialManager.h"

// #include "TGeoVolume.h"
// #include "TGeoManager.h"
// #include "TGeoNode.h"
#include "TFile.h"
#include "TH1F.h"

#include <fstream>

#include "main.h"

using namespace dd4hep;
using namespace dd4hep::rec;

void dumpExampleSteering() ;

namespace {

  struct SDetHelper{
    std::string name ;
    double r0 ;
    double r1 ;
    double z0 ;
    double z1 ;
    TH1F*  hx ;
    TH1F*  hl ;
  };

  /// comput a point on a cylinder (r,z) for a given direction (theta,phi) from the IP
  Vector3D pointOnCylinder( double theta, double r, double z, double phi){
    
    double  theta0 = atan2( r, z ) ;

    Vector3D v = (   theta > theta0      ?
		     Vector3D(       r           , phi , r / tan( theta ) , Vector3D::cylindrical )  :
		     Vector3D( z * tan( theta )  , phi ,      z           , Vector3D::cylindrical )  ) ;
    return v ;
  }

}

int main_wrapper(int argc, char** argv)   {
  
  struct Handler  {
    Handler() { SetErrorHandler(Handler::print); }
    
    static void print(int level, Bool_t abort, const char *location, const char *msg)  {
      if ( level > kInfo || abort ) ::printf("%s: %s\n", location, msg);
    }
    
    static void usage()  {
      std::cout << " usage: materialBudget compact.xml steering.txt" << std::endl 
                << "     -> create histograms with the material budget as seen from the IP within fixed ranges of (rmin, rmax, zmin. zmax)" << std::endl 
                << "        see example steering file for details ..." << std::endl 
		<< "  -x   : dump example steering file " << std::endl
                << std::endl;
      exit(1);
    }

  } _handler;


  if( argc == 2 ){
    std::string dummy = argv[1] ;
    
    if( dummy == "-x" )
      dumpExampleSteering() ;
    else
      Handler::usage();
  }
  
  if( argc != 3 )
    Handler::usage();
  

  // ================== parse steering file ========================================================
  std::string compactFile  =  argv[1];
  std::string steeringFile =  argv[2];
  int nbins = 90 ;
  double phi0 = M_PI / 2. ;
  std::string outFileName("material_budget.root") ;
  std::vector<SDetHelper> subdets ;
    

  std::ifstream infile(steeringFile );

  std::string line;

  while (std::getline(infile, line))
  {
    // ignore empty lines and comments
    if( line.empty() || line.find("#") == 0 )
      continue ;

    std::istringstream iss(line);
    std::string token ;
    iss >> token ;
    
    if( token == "nbins" ){
      iss >> nbins ;
    }
    else if( token == "rootfile" ){
      iss >> outFileName ;
    }
    else if( token == "phi" ){
      iss >> phi0 ;
      phi0 = phi0 / 180. * M_PI ;
    }
    else if( token == "subdet" ){
      SDetHelper det ;
      iss >>  det.name >> det.r0 >> det.z0 >> det.r1 >> det.z1 ;
      subdets.emplace_back( det );
    }

    if ( !iss.eof() || iss.fail() ){
      std::cout << " ERROR parsing line : " << line << std::endl ;
      exit(1) ;
    }
    
  }

  // =================================================================================================
  setPrintLevel(WARNING);

  Detector& description = Detector::getInstance();
  description.fromXML(compactFile ) ;


  //----- open root file and book histograms
  TFile* rootFile = new TFile(outFileName.c_str(),"recreate");

  for(auto& det : subdets){

    std::string hxn(det.name), hxnn(det.name) ;
    std::string hln(det.name), hlnn(det.name) ;
    hxn += "x0" ;
    hxnn += " integrated X0 vs -theta" ;
    det.hx = new TH1F( hxn.c_str(), hxnn.c_str(), nbins, -90. , 0. ) ;

    hln += "lambda" ;
    hlnn += " integrated int. lengths vs -theta" ;
    det.hl = new TH1F( hln.c_str(), hlnn.c_str(), nbins, -90. , 0. ) ;

  }
  //-------------------------
      

  Volume world  = description.world().volume() ;
  
  MaterialManager matMgr( world ) ;
  
  double dTheta = 0.5*M_PI/nbins; // bin size

  std::cout  << "====================================================================================================" << std::endl ;

  std::cout  << "theta:f/" ;
  for(auto& det : subdets){ std::cout  << det.name << "_x0:f/" << det.name << "_lam:f/" ; }
  std::cout  << std::endl ;
  
  for(int i=0 ; i< nbins ;++i){

    double theta = (0.5+i)*dTheta ;

    std::cout << std::scientific << theta << " " ;
    
    for(auto& det : subdets){
      
      Vector3D p0 = pointOnCylinder( theta, det.r0 , det.z0 , phi0  ) ;// double theta, double r, double z, double phi)
      
      Vector3D p1 = pointOnCylinder( theta, det.r1 , det.z1 , phi0  ) ;// double theta, double r, double z, double phi)
      
      const MaterialVec& materials = matMgr.materialsBetween(p0, p1);

      double sum_x0(0.), sum_lambda(0.),path_length(0.);

      for( auto amat : materials ){
	TGeoMaterial* mat =  amat.first->GetMaterial();
	double length = amat.second;
	double nx0 = length / mat->GetRadLen();
	sum_x0 += nx0;
	double nLambda = length / mat->GetIntLen();
	sum_lambda += nLambda;
	path_length += length;
      }

      det.hx->Fill( -theta/M_PI*180. , sum_x0 ) ;
      det.hl->Fill( -theta/M_PI*180. , sum_lambda ) ;

      std::cout  << std::scientific  << sum_x0 << "  " << sum_lambda << "  " ; // << path_length ;

    }
    std::cout  << std::endl ;
  }  
  std::cout  << "====================================================================================================" << std::endl ;


  rootFile->Write();
  rootFile->Close();
   
  return 0;
}



void dumpExampleSteering(){


  std::cout << "# Example steering file for materialBudget  (taken from ILD_l5_v02)" << std::endl ;
  std::cout <<  std::endl ;
  std::cout << "# root output file"  << std::endl ;
  std::cout << "rootfile material_budget.root" << std::endl ;
  std::cout << "# number of bins for polar angle (default 90)" << std::endl ;
  std::cout << "nbins 90" << std::endl ;
  std::cout <<  std::endl ;
  std::cout << "# phi direction in deg (default: 90./y-axis)" << std::endl ;
  std::cout << "phi 90." << std::endl ;
  std::cout <<  std::endl ;
  std::cout << "# names and subdetector ranges given in [rmin,zmin,rmax,zmax] - e.g. for ILD_l5_vo2  (run dumpdetector -d to get numbers... ) " << std::endl ;
  std::cout <<  std::endl ;
  std::cout << "subdet vxd    0. 0. 6.549392e+00 1.450000e+01" << std::endl ;
  std::cout << "subdet sitftd 0. 0. 3.024000e+01 2.211800e+02" << std::endl ;
  std::cout << "subdet tpc    0. 0. 1.692100e+02 2.225000e+02" << std::endl ;
  std::cout << "subdet outtpc 0. 0. 1.769800e+02 2.350000e+02" << std::endl ;
  std::cout << "subdet set    0. 0. 1.775200e+02 2.300000e+02" << std::endl ;

  exit(0);
}
