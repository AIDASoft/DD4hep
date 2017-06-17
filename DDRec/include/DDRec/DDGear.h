#ifndef DDGear_H
#define DDGear_H

#include "DD4hep/LCDD.h"
#include "DD4hep/Detector.h"

#include "gear/GEAR.h"
#include "gearimpl/GearParametersImpl.h"
#include "gearimpl/SimpleMaterialImpl.h"
#include "gear/GearMgr.h"

namespace gear{
  class GearParametersImpl ;
  class GearMgr ;
}

namespace dd4hep {
  namespace rec {
    
  /**
     Simple wrapper class for objects of type GearParametersImpl that
     can be added to DetElements with the extension mechanism.
     There can only be one such object added to any given DetElement 
     @author  F.Gaede, DESY
     @version $Id$
  */

  class GearHandle  { 
    
  protected:
    gear::GearParametersImpl* _gObj ;
    std::string _name ;
    std::vector< gear::SimpleMaterialImpl > _materials ;
    
  public :
    /** Default c'tor  - only used by DD4hep extenbsion mechanism.*/ 
    GearHandle() : _gObj( 0 ) , _name( "UNKNOWN" ) {}
    
    /** User c'tor - specify a name (should be the canonical name used in gear, eg. TPCParameters, SITParameters, etc.) and
	the GearParametersImpl object */
    GearHandle( gear::GearParametersImpl* gearObj, const std::string& nam  ) : _gObj( gearObj ) , _name( nam ) {}
    
    /** D'tor deletes GearParametersImpl object if ownerhsip has not been taken away */
    virtual ~GearHandle() { 
      if( _gObj) 
	delete _gObj ; 
    }
    
    /** Name of GearParametersImpl object - should be the canonical name used in gear, eg. TPCParameters, SITParameters, etc. */
    const std::string& name() { return _name ; }
    
    /** Get GearParametersImpl object */
    gear::GearParametersImpl* gearObject() { return _gObj ; } 
    
    /** Get GearParametersImpl object _and_ transfer ownership */
    gear::GearParametersImpl* takeGearObject() { 
      gear::GearParametersImpl* obj = _gObj ; 
      _gObj = 0 ;
      return obj ; 
    } 
    

    /// add a SimpleMaterial object 
    void addMaterial(const std::string nam, double A, double Z, double density, double radLen, double intLen){

      _materials.push_back( gear::SimpleMaterialImpl (nam, A,  Z,  density, radLen,  intLen) ) ;
    }

    /// get all materials assigned to this wrapper
    const std::vector< gear::SimpleMaterialImpl >& materials() { return _materials ; }


    /** dummy implementation of required c'tors to allow using the extension mechanism */
    GearHandle(const DetElement& /*d*/) : _gObj(0) {}
    GearHandle(const GearHandle& /*c*/,const DetElement& /*det*/) : _gObj(0)  {}
  } ;
  

  //===============================================================================================================

  /** Factory method that creates a GearMgr object pobulated with the GearParametersImpl objects from
      all (top level) DetElements in the lcdd object
      @author  F.Gaede, DESY
      @version $Id$
  */
  gear::GearMgr* createGearMgr( LCDD& lcdd , const std::string& pluginName="GearForILD" ) ;

  }}

namespace DD4hep { namespace DDRec { using namespace dd4hep::rec  ; } }  // bwd compatibility for old namsepaces

#endif
