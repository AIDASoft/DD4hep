//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================

namespace { 
  struct UserData { 
    int dimension ;
  }; 
}

// Framework include files
#define SURFACEINSTALLER_DATA UserData
#define DD4HEP_USE_SURFACEINSTALL_HELPER Lhe_SiTrackerBarrelSurfacePlugin
#include "DD4hep/SurfaceInstaller.h"

namespace{
  template <> void Installer<UserData>::handle_arguments(int argc, char** argv)   {
    for(int i=0; i<argc; ++i)  {
      char* ptr = ::strchr(argv[i],'=');
      if ( ptr )  {
        std::string name( argv[i] , ptr ) ;
        double value = dd4hep::_toDouble(++ptr);
        if( name=="dimension" ) data.dimension = value ; 
        std::cout << "DD4hep_SiTrackerBarrelSurfacePlugin: argument[" << i << "] = " << name 
                  << " = " << value << std::endl;
      }
    }
  }  
  
  /// Install measurement surfaces
  template <typename UserData> 
    void Installer<UserData>::install(dd4hep::DetElement component, dd4hep::PlacedVolume pv)   {
    dd4hep::Volume comp_vol = pv.volume();
    if ( comp_vol.isSensitive() )  {  
      dd4hep::Volume mod_vol  = parentVolume(component);
      dd4hep::Box mod_shape(mod_vol.solid()), comp_shape(comp_vol.solid());
      
      if ( !comp_shape.isValid() || !mod_shape.isValid() )   {
        invalidInstaller("Components and/or modules are not boxes -- invalid shapes");
      }
      else if ( !handleUsingCache(component,comp_vol) )  {
        const double* trans = placementTranslation(component);
        double half_module_thickness = mod_shape->GetDZ();
        double sensitive_z_position  = trans[2];
        double outer_thickness = half_module_thickness + sensitive_z_position;
        double inner_thickness = half_module_thickness - sensitive_z_position;
        //Surface is placed at the center of the volume, no need to shift origin
        //Make sure u,v,n form a right-handed coordinate system, v along the final z
        Vector3D u(-1.,0.,0.), v(0.,-1.,0.), n(0.,0.,1.), o(0.,0.,0.);
	
        Type type( Type::Sensitive ) ;

        if( data.dimension == 1 ) {
          type.setProperty( Type::Measurement1D , true ) ;
        } else if( data.dimension != 2 ) {
          throw std::runtime_error("**** Lhe_SiTrackerBarrelSurfacePlugin: no or wrong "
                                   "'dimension' argument given - has to be 1 or 2") ;
        }
        VolPlane surf(comp_vol, type, inner_thickness, outer_thickness, u, v, n, o);
        addSurface(component,surf);
      }
    }
  }
  

}// namespace
