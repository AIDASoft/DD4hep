#include "DDSense/DDSensitiveManager.h"
#include "DDSense/G4SensitiveDetector.h"

#include "DD4hep/BitField64.h"  // provides StringTokenizer

#include <dlfcn.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>


/** DD4hep namespace declaration */
namespace DD4hep {
  /** DDSense namespace declaration  */
  namespace Simulation  {
    
    const std::string DDSensitiveManager::DLL = "DD4hep_SD_DLL"  ;

   typedef std::vector<std::string> StringVec ;

    DDSensitiveManager* DDSensitiveManager::_me = 0 ;
    
    DDSensitiveManager* DDSensitiveManager::instance(){
      if( !_me){
	_me = new DDSensitiveManager ;
	
	// now load all plugins defined in DD4HepSD_DLL:
	//	_me->loadLibraries() ;
      }
      return _me ;
    }

    DDSensitiveManager::DDSensitiveManager(){
    }
    

    DDSensitiveManager::~DDSensitiveManager(){
    }

    void DDSensitiveManager::registerSD( G4SensitiveDetector*s ){

      //      std::cout << "  DDSensitiveManager::registerSD( " << s->typeName() << ")" << std::endl ;
      
      SDMap::iterator it = _map.find( s->typeName() ) ;
      if( it == _map.end() )
	_map.insert( std::make_pair( s->typeName() ,  s ) ) ; 
    }
    
    

    std::vector<std::string> DDSensitiveManager::getSDTypes() {

      std::vector< std::string> sdtypes ;
      for( SDMap:: iterator it = _map.begin() ; it != _map.end() ; ++it ){

	sdtypes.push_back( it->first ) ;
      }

      return sdtypes ;
    }

  
    void DDSensitiveManager::loadLibraries(){
      
      StringVec libs ;
      StringTokenizer t( libs, ':' ) ;
      
      std::string sds("") ;
      
      char* var =  std::getenv( DLL.c_str() ) ;
      if( var != 0 ) {
        sds = var ;
      } else {
        std::cout << std::endl << "******** You have no " << DLL << " variable in your environment "
	  " - so no G4SensitiveDetectors will be loaded. ! ******* " << std::endl << std::endl ;
      }
      
      std::for_each( sds.begin(), sds.end(), t ) ;
      
      bool loadError=false;
      
      std::set<std::string> checkDuplicateLibs;
      

      for( unsigned i=0,N=libs.size() ; i<N ; ++i){

        const std::string& libName = libs[i] ;

        size_t idx;
        idx = libName.find_last_of("/");
        // the library basename, i.e. /path/to/libBlah.so --> libBlah.so
        std::string libBaseName( libName.substr( idx + 1 ) );

        char *real_path = realpath(libName.c_str(), NULL);

        if( real_path != NULL ){
	  std::cout << " Loading shared library : " << real_path << " ("<< libBaseName << ") " << std::endl ;

	  // use real_path
	  free(real_path);
        }
        else{
	  std::cout << "Loading shared library : " << libName << " ("<< libBaseName << ") " << std::endl ;
        }
        
        if( checkDuplicateLibs.find( libBaseName ) == checkDuplicateLibs.end() ){
	  checkDuplicateLibs.insert( libBaseName ) ;
        }
        else{
	  std::cout << std::endl << "  ERROR loading shared library : " << libName << std::endl
		    << "    ->    Trying to load DUPLICATE library " << std::endl << std::endl ;
	  loadError=true;
        }


        if( ! loadError ){

	  //void* libPointer  = dlopen( libName.c_str() , RTLD_NOW) ;
	  //void* libPointer  = dlopen( libName.c_str() , RTLD_LAZY ) ;
	  //void* libPointer  = dlopen( libName.c_str() , RTLD_NOW | RTLD_GLOBAL) ;
	  void* libPointer  = dlopen( libName.c_str() , RTLD_LAZY | RTLD_GLOBAL) ;

	  if( libPointer == 0 ){
	    std::cout << std::endl << " ERROR loading shared library : " << libName << std::endl
		      << "    ->    "   << dlerror()  << std::endl << std::endl ;
	    loadError=true;
	  }
	  // else{
	  //   libs.push_back( libPointer ) ;
	  // }
        }


      }// loop over libs

    }
    //--------------------------------------


    
  } //namespace 
} //namespace 

