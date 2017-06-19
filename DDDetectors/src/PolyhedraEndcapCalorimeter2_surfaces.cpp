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
// Framework include files
#define  DD4HEP_USE_SURFACEINSTALL_HELPER  DD4hep_PolyhedraEndcapCalorimeterSurfacePlugin
#include "DD4hep/SurfaceInstaller.h"

/// Install measurement surfaces
template <typename UserData> 
void Installer<UserData>::install(dd4hep::DetElement component, dd4hep::PlacedVolume pv)   {
  dd4hep::Volume comp_vol = pv.volume();
  if ( comp_vol.isSensitive() )  {  
    dd4hep::Volume mod_vol = parentVolume(component);
    dd4hep::PolyhedraRegular comp_shape(comp_vol.solid()), mod_shape(mod_vol.solid());

    if ( !comp_shape.isValid() || !mod_shape.isValid() )   {
      invalidInstaller("Components and/or modules are not Trapezoid -- invalid shapes");
    }
    else if ( !handleUsingCache(component,comp_vol) )  {
      dd4hep::DetElement par = component.parent();
      const TGeoHMatrix& m = par.nominal().worldTransformation();
      double dz = m.GetTranslation()[2];
      const double* trans = placementTranslation(component);
      double half_mod_thickness  = (mod_shape->GetZ(1)-mod_shape->GetZ(0))/2.0;
      double half_comp_thickness = (comp_shape->GetZ(1)-comp_shape->GetZ(0))/2.0;
      double si_position         = trans[2]+half_mod_thickness;
      double outer_thickness = half_mod_thickness - si_position;
      double inner_thickness = half_mod_thickness + si_position;
      Vector3D u(1.,0.,0.), v(0.,1.,0.), n(0.,0.,1.), o(100.,100.,0.);
      std::cout << " Module:    " << mod_shape.toString() << std::endl;
      std::cout << " Component: " << comp_shape.toString() << std::endl;
      std::cout << "dz:" << dz << " Si-pos:" << si_position 
                << " Mod-thickness:" << half_mod_thickness 
                << " Comp-thickness:" << half_comp_thickness 
                << std::endl;
      VolPlane surf(comp_vol,Type(Type::Sensitive,Type::Measurement1D),
                    inner_thickness, outer_thickness, u, v, n, o);
      addSurface(component,surf);
    }
  }
}
