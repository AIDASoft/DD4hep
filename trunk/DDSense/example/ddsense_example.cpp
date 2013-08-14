// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  @Author     : F.Gaede, DESY
//====================================================================

#include "DDSense/DDSensitiveManager.h"
#include "DDSense/G4SensitiveDetector.h"

#include <iostream>


/** Simple test and demonstration program for simple plugin mechanism for SensitiveDetector classes.
 */
int main(){


   DD4hep::Simulation::DDSensitiveManager* mgr = DD4hep::Simulation::DDSensitiveManager::instance() ;


   std::cout << " loading plugins as specified in env variable " 
	     << DD4hep::Simulation::DDSensitiveManager::DLL 
	     << std::endl ;
   
   mgr->loadLibraries() ;

   std::cout << " get G4SensitiveDetectors types: " << std::endl ;

   std::vector<std::string> sdt = mgr->getSDTypes() ;

   
   for(unsigned i=0,N=sdt.size() ; i<N ; ++i){
     std::cout << "      " << sdt[i] << std::endl ;
   }


   if( sdt.size() ) {

     std::cout << "  get instance for type " <<  sdt[0]   <<  std::endl ;
   

     DD4hep::Simulation::G4SensitiveDetector* sd = mgr->getSD( sdt[0]  ) ;
   

     if( sd ){

       std::cout << "  and create a new instance and configure it ..."  <<  std::endl ;


       DD4hep::Simulation::G4SensitiveDetector* sd_1 =  sd->newInstance() ;
       
       DD4hep::Simulation::G4SensitiveDetector::ParameterMap params;

       params["Readout"] = "GridXYZ" ;
       params["EnergyCut"] = "0.1*keV" ;
       
       sd_1->init( params ) ;
       

       std::cout <<  sd_1->toString() <<  std::endl ;
     

       delete sd_1 ;

     }

   } else {

     std::cout << " no SDs loaded - make sure you set " << DD4hep::Simulation::DDSensitiveManager::DLL << " correctly ...." << std::endl ;

   }
}
