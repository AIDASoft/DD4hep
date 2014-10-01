#ifndef DDGear_H
#define DDGear_H

#include "DD4hep/Detector.h"

// #include "gear/GEAR.h"
// #include "gearimpl/GearParametersImpl.h"
// #include "gear/GearMgr.h"

namespace gear{
  class GearParametersImpl ;
  class GearMgr ;
}

namespace DD4hep {

  /**
     Simple wrapper class for objects of type GearParametersImpl that
     can be added to DetElements with the extension mechanism.
     There can only be one such object added to any given DetElement 
     @author  F.Gaede, DESY
     @version $Id$
  */

  class GearHandle  { //: public gear::GearParametersImpl {
    
  protected:
    gear::GearParametersImpl* _gObj ;
    std::string _name ;
    
  public :
    /** Default c'tor  - only used by DD4hep extenbsion mechanism.*/ 
    GearHandle() : _gObj( 0 ) , _name( "UNKNOWN" ) {}
    
    /** User c'tor - specify a name (should be the canonical name used in gear, eg. TPCParameters, SITParameters, etc.) and
	the GearParametersImpl object */
    GearHandle( gear::GearParametersImpl* gearObj, const std::string& name  ) : _gObj( gearObj ) , _name( name ) {}
    
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
    
    /** dummy implementation of required c'tors to allow using the extension mechanism */
    GearHandle(const Geometry::DetElement& d) : _gObj(0) {}
    GearHandle(const GearHandle& c,const Geometry::DetElement& det) : _gObj(0)  {}
  } ;
  

  //===============================================================================================================

  /** Factory method that creates a GearMgr object pobulated with the GearParametersImpl objects from
      all (top level) DetElements in the lcdd object
      @author  F.Gaede, DESY
      @version $Id$
  */
  gear::GearMgr* createGearMgr( Geometry::LCDD& lcdd , const std::string& pluginName="GearForILD" ) ;

}

#endif
