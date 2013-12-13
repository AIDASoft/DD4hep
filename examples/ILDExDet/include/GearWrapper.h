#ifndef GearWrapper_H
#define GearWrapper_H
//====================================================================

#include "DD4hep/Detector.h"
//#include "DD4hepExceptions.h" 

#include "gear/GEAR.h"
#include "gear/TPCParameters.h"
#include "gearimpl/TPCParametersImpl.h"
#include "gearimpl/ZPlanarParametersImpl.h"

namespace DD4hep {



  
  //// generic wrapper class for gear::XXXParametersImpl:
  //template <class GEAR>
  //struct GearWrapper : public GEAR {
  //  // required c'tors for extension mechanism
  //  GearWrapper(const Geometry::DetElement& d){}
  //  GearWrapper(const GearWrapper<GEAR>& c,const Geometry::DetElement& det){}
  //} ;
  //
  //// specialized c'tor
  //template <>
  //GearWrapper<gear::TPCParametersImpl>( double maxDriftLength, int coordinateType ) 
  //: gear::TPCParametersImpl(  maxDriftLength, coordinateType){ }
  //
  //typedef GearWrapper<gear::TPCParametersImpl> GearTPCParameters ;

  // wrapper class for gear::TPCParametersImpl:
  struct GearTPCParameters : public gear::TPCParametersImpl {
    // required c'tors for extension mechanism
    GearTPCParameters(const Geometry::DetElement& d){}
    GearTPCParameters(const GearTPCParameters& c,const Geometry::DetElement& det){}
    GearTPCParameters( double maxDriftLength, int coordinateType ) : gear::TPCParametersImpl(  maxDriftLength, coordinateType){}
  } ;
  
  
  struct GearZPlanarParameters : public gear::ZPlanarParametersImpl {
    
    // required c'tors for extension mechanism
    
    GearZPlanarParameters(const Geometry::DetElement& d) : gear::ZPlanarParametersImpl(ZPlanarParametersImpl::CMOS,0,0,0,0,0) {}
    
    GearZPlanarParameters(const GearZPlanarParameters& c,const Geometry::DetElement& det)
      : gear::ZPlanarParametersImpl(ZPlanarParametersImpl::CMOS,0,0,0,0,0) {}
    
    GearZPlanarParameters( int type=ZPlanarParametersImpl::CMOS, 
   			   double shellInnerRadius=0., double shellOuterRadius=0., 
   			   double shellHalfLength=0.,  double shellGap=0., double shellRadLength=0.) 
      : gear::ZPlanarParametersImpl( type, shellInnerRadius, shellOuterRadius,
   				     shellHalfLength, shellGap, shellRadLength ){}
  } ;
  
}

#endif
