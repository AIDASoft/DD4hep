#include "DDSense/G4SensitiveDetector.h"
#include "DDSense/DDSensitiveManager.h"

#include <sstream>

/** DD4hep namespace declaration */
namespace DD4hep {
  /** DDSense namespace declaration  */
  namespace Simulation  {
    

    G4SensitiveDetector::G4SensitiveDetector(const std::string& typeName) : _typeName( typeName ) {

      DDSensitiveManager::instance()->registerSD( this ) ;
    }


    void G4SensitiveDetector::init( const ParameterMap&  parameters ){

      _params = parameters ; // copy parameter map 
    }


    G4SensitiveDetector::~G4SensitiveDetector(){
    }

    
    std::string G4SensitiveDetector::toString(){

      std::stringstream s ;

      s << "---- G4SensitiveDetector - type " << typeName() << std::endl ; 

      for( ParameterMap::iterator it = _params.begin() ; it != _params.end() ; ++it ){

	s << "  " << it->first << ":  " << it->second << std::endl ;
      }
      s << "----" << std::endl ;
      
      return s.str() ;
    }


  } //namespace 
} //namespace 

