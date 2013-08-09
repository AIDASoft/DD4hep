// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  @Author     : F.Gaede, DESY
//
//====================================================================

#ifndef DD4hep_Simulation_DDSensitiveManager_h
#define DD4hep_Simulation_DDSensitiveManager_h

// C/C++ include files
#include <string>
#include <map>
#include <vector>

/** DD4hep namespace declaration */
namespace DD4hep {  
  /** DDSense namespace declaration  */  
  namespace Simulation {
    
    class G4SensitiveDetector ;

    /**Singleton class that holds all known plugins of DDSensitiveDetector objects ...
     */
    class DDSensitiveManager{
      
    public:

      typedef std::map< std::string, G4SensitiveDetector*> SDMap ;

      // name of the env variable
      static const std::string DLL ;

      /** Return the instance of this manager.
       */
      static DDSensitiveManager* instance() ;
      
      /** d'tor
       */
      virtual ~DDSensitiveManager() ;
      

      /** register a G4SensitiveDetector */
      void registerSD(  G4SensitiveDetector* ) ;


      /** Load libraries with plugins from files specified in environment variable 'DD4hep_SD_DLL' */
      void loadLibraries() ;


      /** Return a list of registered G4SensitiveDetector types */
      std::vector<std::string> getSDTypes() ;


      /** Get a pointer to the stored SD - 0 if type hast not been registered */
      inline G4SensitiveDetector* getSD( const std::string& typeName ){

	SDMap::iterator i = _map.find( typeName ) ;

	return ( i != _map.end() ? i->second  : 0 ) ;
      } 



    private:
      /** private c'tor */
      DDSensitiveManager() ;
      
 

      SDMap _map ;

      static DDSensitiveManager* _me ;
    } ;
    
    
  } // namespace
} // namespace

#endif
