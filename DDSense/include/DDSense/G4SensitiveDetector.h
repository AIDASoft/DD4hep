// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  @Author     : F.Gaede, DESY
//
//====================================================================

#ifndef DD4hep_Simulation_G4SensitiveDetector_h
#define DD4hep_Simulation_G4SensitiveDetector_h

// C/C++ include files
#include <string>
#include <map>

/** DD4hep namespace declaration */
namespace DD4hep {  
  /** DDSense namespace declaration  */  
  namespace Simulation {
    
    class DDSensitiveManager ;


    /** Base class for G4VSensitiveDetectors used with DD4Hep.
     */
    class G4SensitiveDetector{

    protected :
      /** c'tor */
      G4SensitiveDetector() ;
      
    public:
      
      typedef std::map< std::string, std::string > ParameterMap ;
      
      /**only public c'tor - initialize with unique tpe name */
      G4SensitiveDetector( const std::string& typeName) ; 
      
      /** Return unique type name (typically the class name)
       */
      inline const std::string& typeName() const { return _typeName ; }


     /**Implementation classes need to provide a factory method:
       */ 
      virtual G4SensitiveDetector* newInstance() const =0 ;

      /** Implementation classes can overwrite this to initialize from named parameters
       */
      virtual void init( const ParameterMap&  parameters ) ;

     /** d'tor */
      virtual ~G4SensitiveDetector() ;


      /** Create String with parameters 
       */
      std::string toString() ;

    protected:
      
      ParameterMap _params ;

    private:
      std::string _typeName ;
  
    } ;
    
  } // namespace
} // namespace

#endif
