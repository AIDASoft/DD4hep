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
    float length ;
    float radius ;
    float phi0 ;
    int   symmetry ;
    int   systemID ;
    std::string encoding ;

  }; 
}

// Framework include files
#define SURFACEINSTALLER_DATA UserData
#define DD4HEP_USE_SURFACEINSTALL_HELPER DD4hep_CaloFaceBarrelSurfacePlugin
#include "DD4hep/SurfaceInstaller.h"
#include "DDRec/DetectorData.h"
#include "DDRec/Surface.h"
#include "DDSegmentation/BitField64.h"

namespace{
  
  /// helper class for a planar surface placed into a polyhedral calorimeter barrel 
  class CaloBarrelPlaneImpl : public  DD4hep::DDRec::VolPlaneImpl   {
    double _length, _width ;

  public:
    /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
    CaloBarrelPlaneImpl( DDSurfaces::SurfaceType typ,
                         double thickness_inner ,double thickness_outer, 
                         DDSurfaces::Vector3D u_val ,DDSurfaces::Vector3D v_val ,
                         DDSurfaces::Vector3D n_val , DDSurfaces::Vector3D o_val, 
                         DD4hep::Geometry::Volume vol, int id_val ) :
      DD4hep::DDRec::VolPlaneImpl( typ, thickness_inner,thickness_outer, u_val, v_val, n_val, o_val, vol, id_val),
      _length(0),_width(0) {}
    
    void setData( double length, double width){
      _length = length ;
      _width = width ;
    }

    void setID( DD4hep::long64 id_val ) { _id = id_val ; }
    
    // overwrite to include points inside the inner radius of the barrel 
    bool insideBounds(const DDSurfaces::Vector3D& point, double epsilon) const {
      DDSurfaces::Vector2D uvVec = globalToLocal( point ) ;
      
      return ( std::abs ( distance( point ) ) < epsilon )  &&  
        std::abs( uvVec[0] ) < _width/2. &&  std::abs( uvVec[1] ) < _length/2. ; 
    }
    
    /// create outer bounding lines for the given symmetry of the polyhedron
    virtual std::vector< std::pair<DDSurfaces::Vector3D, DDSurfaces::Vector3D> > getLines(unsigned){
      
      std::vector< std::pair<DDSurfaces::Vector3D, DDSurfaces::Vector3D> >  lines ;
      
      lines.push_back( std::make_pair( origin()+_width/2.*u()+_length/2.*v(), origin()-_width/2.*u()+_length/2.*v() ) ) ;
      lines.push_back( std::make_pair( origin()-_width/2.*u()+_length/2.*v(), origin()-_width/2.*u()-_length/2.*v() ) ) ;
      lines.push_back( std::make_pair( origin()-_width/2.*u()-_length/2.*v(), origin()+_width/2.*u()-_length/2.*v() ) ) ;
      lines.push_back( std::make_pair( origin()+_width/2.*u()-_length/2.*v(), origin()+_width/2.*u()+_length/2.*v() ) ) ;
      
      return lines; 
    }
  };
  
  typedef DD4hep::DDRec::VolSurfaceHandle<CaloBarrelPlaneImpl> CaloBarrelPlane ;

  
  template <> void Installer<UserData>::handle_arguments(int argc, char** argv)   {
    for(int i=0; i<argc; ++i)  {
      char* ptr = ::strchr(argv[i],'=');
      if ( ptr )  {
        std::string name( argv[i] , ptr ) ;
        double value = DD4hep::Geometry::_toDouble(++ptr);
        
        std::cout << "DD4hep_CaloFaceBarrelSurfacePlugin: argument[" << i << "] = " << name 
                  << " = " << value << std::endl;

        if(      name=="length"    ) data.length     = value ; 
        else if( name=="radius"  ) data.radius   = value ; 
        else if( name=="phi0"    ) data.phi0     = value ; 
        else if( name=="symmetry") data.symmetry = value ; 
        else if( name=="systemID") data.systemID   = value ; 
        else if( name=="encoding") data.encoding = ptr ; 
        else {
          std::cout << "DD4hep_CaloFaceBarrelSurfacePlugin: WARNING unknown parameter: " << name << std::endl ;
        }
      }
    }
  }  
  
  /// Install measurement surfaces
  template <typename UserData> 
    void Installer<UserData>::install(DetElement component, PlacedVolume pv)   {
    
    Volume comp_vol = pv.volume();
    
    double length   = data.length ;
    double symmetry = data.symmetry ;
    double radius   = data.radius ;
    double phi0     = data.phi0 ;  

    //    if( symmetry ) radius /= cos( M_PI / symmetry ) ; 
    

    double width = 2. * radius * tan( M_PI / symmetry ) ;

    double inner_thickness = 1e-6 ;
    double outer_thickness = 1e-6 ;
    
    std::cout << "DD4hep_CaloFaceBarrelSurfacePlugin: install tracking surfaces for : " 
              << component.name() << std::endl ;


    DD4hep::DDSegmentation::BitField64 bf( "system:5,side:-2,layer:9,module:8,sensor:8" ) ;
    // DD4hep::DDSegmentation::BitField64 bf( data.encoding ) ;

    bf["system"] = data.systemID ;
    

    double alpha = ( symmetry ? 2.* M_PI / symmetry : 0. ) ;
      
    for(unsigned i=0 ; i < symmetry ; ++i){

      bf["module"] =  i ; 

      double gam = phi0 + alpha/2. + i*alpha;

      Vector3D 
        u( cos(gam+M_PI/2.), sin(gam+M_PI/2.), 0. ),
        v(          0.     ,        0.       , 1. ), 
        n(        cos(gam) ,        sin(gam) , 0. ),
        o( radius*cos(gam) , radius*sin(gam) , 0. );
      
      CaloBarrelPlane surf(comp_vol,Type(Type::Helper,Type::Sensitive), inner_thickness, outer_thickness, u, v, n, o);

      surf->setData( length, width ) ;
      surf->setID( bf.getValue() ) ;
      
      addSurface(component,surf);

    }      

    // stop scanning the hierarchy any further
    stopScanning() ;
  }
  

}// namespace
