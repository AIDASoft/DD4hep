// $Id: $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : F.Gaede, DESY
//
//====================================================================
#ifndef DD4HEP_G4SDFactory_H
#define DD4HEP_G4SDFactory_H


#include <string>

namespace DD4hep { namespace Geometry { class LCDD ; } }
class G4VSensitiveDetector ;

/*  DD4hep namespace declaration */
namespace DD4hep { namespace Simulation {
    
    
    /** @class G4SDFactory G4SDFactory.h DDG4/G4SDFactory.h
     *  Abstract factory base class for createion of G4VSensitiveDetectors
     * 
     * @author  F.Gaede, DESY
     * @version $Id:$
     */
    class G4SDFactory {

    public:      
      
      /** Implement this to create a new SD with name, initialized with the information in lcdd */
      virtual G4VSensitiveDetector*  createSD( const std::string& name,  DD4hep::Geometry::LCDD& lcdd )=0 ;
      
    } ;	
    

    /** Simpleimplementation of factory for SDs that have a c'tor of the form 
     *  T( const std::string& name,  DD4hep::Geometry::LCDD& lcdd )
     */
    template <class  T>
    class SimpleG4SDFactory : public G4SDFactory {

      virtual G4VSensitiveDetector*  createSD( const std::string& name,  DD4hep::Geometry::LCDD& lcdd ){
	return new T( name,  lcdd ) ;
      }
    } ;


  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4SENSITIVEDETECTOR_H
