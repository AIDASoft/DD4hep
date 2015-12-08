// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede DESY
//
//==========================================================================

#include <string>

namespace { 
  struct UserData { 
    float zpos ;
    float radius ;
    float phi0 ;
    int   symmetry ;
    int   systemID ;
    std::string encoding ;
  }; 
}

// Framework include files
#define SURFACEINSTALLER_DATA UserData
#define DD4HEP_USE_SURFACEINSTALL_HELPER DD4hep_CaloFaceEndcapSurfacePlugin
#include "DD4hep/SurfaceInstaller.h"
#include "DDRec/DetectorData.h"
#include "DDRec/Surface.h"
#include "DDSegmentation/BitField64.h"

namespace{
  
  /// helper class for a planar surface placed into a polyhedral calorimeter endcap 
  class CaloEndcapPlaneImpl : public  DD4hep::DDRec::VolPlaneImpl{
    double  _r ;
    double  _phi0 ;
    unsigned _sym ;
  public:
    /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
    CaloEndcapPlaneImpl( DDSurfaces::SurfaceType typ,
                         double thickness_inner ,double thickness_outer, 
                         DDSurfaces::Vector3D u_val ,DDSurfaces::Vector3D v_val ,
                         DDSurfaces::Vector3D n_val , DDSurfaces::Vector3D o_val, 
                         DD4hep::Geometry::Volume vol, int id_val ) :
      DD4hep::DDRec::VolPlaneImpl( typ, thickness_inner,thickness_outer, u_val, v_val, n_val, o_val, vol, id_val),
      _r(0),_phi0(0),_sym(0) {}
    
    void setData( double radius, double phi0, unsigned symmetry){
      _r = radius ;
      _phi0 = phi0 ;
      _sym = symmetry ;
    }
    void setID( DD4hep::long64 id_val ) { _id = id_val; }
    
    // overwrite to include points inside the inner radius of the endcap 
    bool insideBounds(const DDSurfaces::Vector3D& point, double epsilon) const {
      
      double ri = _r * cos(  2.* M_PI / _sym ) ;
      
      return ( std::abs ( distance( point ) ) < epsilon )  && 
        ( (point.rho() < ri ) || 
          ( volume()->GetShape()->Contains( const_cast<double*> (point.const_array() ) ) ) ) ;
    }

    /// create outer bounding lines for the given symmetry of the polyhedron
    virtual std::vector< std::pair<DDSurfaces::Vector3D, DDSurfaces::Vector3D> > getLines(unsigned){
      
      std::vector< std::pair<DDSurfaces::Vector3D, DDSurfaces::Vector3D> >  lines ;
      
      double alpha = ( _sym ? 2.* M_PI / _sym : 0. ) ;
      
      for(unsigned i=0 ; i < _sym ; ++i){
        double gam0 =    i  * alpha + _phi0 ;
        double gam1 = (i+1) * alpha + _phi0 ;
        lines.push_back( std::make_pair( DDSurfaces::Vector3D( _r*cos(gam0), _r*sin(gam0), origin().z()  ), 
                                         DDSurfaces::Vector3D( _r*cos(gam1), _r*sin(gam1), origin().z()  ) ) ) ;
      }
      return lines; 
    }
  };
  
  typedef DD4hep::DDRec::VolSurfaceHandle<CaloEndcapPlaneImpl> CaloEndcapPlane ;

  
  template <> void Installer<UserData>::handle_arguments(int argc, char** argv)   {
    for(int i=0; i<argc; ++i)  {
      char* ptr = ::strchr(argv[i],'=');
      if ( ptr )  {
        std::string name( argv[i] , ptr ) ;
        double value = DD4hep::Geometry::_toDouble(++ptr);
        
        std::cout << "DD4hep_CaloFaceEndcapSurfacePlugin: argument[" << i << "] = " << name 
                  << " = " << value << std::endl;

        if(      name=="zpos"    ) data.zpos     = value ; 
        else if( name=="radius"  ) data.radius   = value ; 
        else if( name=="phi0"    ) data.phi0     = value ; 
        else if( name=="symmetry") data.symmetry = value ; 
        else if( name=="systemID") data.systemID   = value ; 
        else if( name=="encoding") data.encoding = value ; 
        else {
          std::cout << "DD4hep_CaloFaceEndcapSurfacePlugin: WARNING unknown parameter: " << name << std::endl ;
        }
      }
    }
  }  
  
  /// Install measurement surfaces
  template <typename UserData> 
    void Installer<UserData>::install(DetElement component, PlacedVolume pv)   {
    
    Volume comp_vol = pv.volume();
    

    double zpos     = data.zpos ;
    double symmetry = data.symmetry ;
    double radius   = data.radius ;
    double phi0     = data.phi0 ;  

    if( symmetry ) radius /= cos( M_PI / symmetry ) ; 
    

    double inner_thickness = 1e-6 ;
    double outer_thickness = 1e-6 ;
    
    std::cout << "DD4hep_CaloFaceEndcapSurfacePlugin: install tracking surfaces for : " 
              << component.name() << std::endl ;


    DD4hep::DDSegmentation::BitField64 bf( "system:5,side:-2,layer:9,module:8,sensor:8" ) ;
    // DD4hep::DDSegmentation::BitField64 bf( data.encoding ) ;

    bf["system"] = data.systemID ;
    bf["side"]   =  1 ; 
    

    // shift position of origin of helper plane to pick up Air instead of vacuum 
    Vector3D u(1.,0.,0.), v(0.,1.,0.), n(0.,0.,1.), o(0., 0.5*radius , zpos );
    
    CaloEndcapPlane surf_pz(comp_vol,Type(Type::Helper,Type::Sensitive), inner_thickness, outer_thickness, u, v, n, o);

    surf_pz->setData( radius, phi0, symmetry ) ;
    surf_pz->setID( bf.getValue() ) ;

    addSurface(component,surf_pz);

    CaloEndcapPlane surf_nz(comp_vol,Type(Type::Helper,Type::Sensitive), inner_thickness, outer_thickness, u, v, n, -1*o );

    bf["side"]   =  -1 ; 
    surf_nz->setData( radius, phi0, symmetry ) ;
    surf_nz->setID( bf.getValue() ) ;

    addSurface(component,surf_nz);

    // stop scanning the hierarchy any further
    stopScanning() ;
  }
  

}// namespace
