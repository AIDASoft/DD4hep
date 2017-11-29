//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  Author     : M.Frank
//  Modified   : E.Pilicer ( tube + elliptical pipe layout )
//             : P.Kostka   ( more refined version circ-ell )
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/DetType.h"
#include "DD4hep/Printout.h"

#include "DDRec/DetectorData.h"
#include "DDRec/Surface.h"

#include "XML/Utilities.h"

#include <cmath>
#include <map>
#include <string>

using dd4hep::Transform3D;
using dd4hep::Position;
using dd4hep::RotationY;
using dd4hep::RotateY;
using dd4hep::ConeSegment;
using dd4hep::SubtractionSolid;
using dd4hep::Material;
using dd4hep::Volume;
using dd4hep::Solid;
using dd4hep::Tube;
namespace units = dd4hep;
using dd4hep::rec::Vector3D;
using dd4hep::rec::VolCylinder;
using dd4hep::rec::VolCone;
using dd4hep::rec::SurfaceType;

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

/// helper class for a simple cylinder surface parallel to z with a given length - used as IP layer
class SimpleCylinderImpl : public  dd4hep::rec::VolCylinderImpl{
  double _half_length ;
public:
  /// standard c'tor with all necessary arguments - origin is (0,0,0) if not given.
  SimpleCylinderImpl( dd4hep::Volume vol, SurfaceType type,
		      double thickness_inner ,double thickness_outer,  Vector3D origin ) :
    dd4hep::rec::VolCylinderImpl( vol,  type, thickness_inner, thickness_outer,   origin ),
    _half_length(0){
  }
  void setHalfLength( double half_length){
    _half_length = half_length ;
  }
  void setID( dd4hep::long64 id ) { _id = id ;
  }
  // overwrite to include points inside the inner radius of the barrel
  bool insideBounds(const Vector3D& point, double epsilon) const {
    return ( std::abs( point.rho() - origin().rho() ) < epsilon && std::abs( point.z() ) < _half_length ) ;
  }

  virtual std::vector< std::pair<Vector3D, Vector3D> > getLines(unsigned nMax=100){

    std::vector< std::pair<Vector3D, Vector3D> >  lines ;

    lines.reserve( nMax ) ;

    Vector3D zv( 0. , 0. , _half_length ) ;
    double r = _o.rho() ;

    unsigned n = nMax / 4 ;
    double dPhi = 2.* ROOT::Math::Pi() / double( n ) ;

    for( unsigned i = 0 ; i < n ; ++i ) {

      Vector3D rv0(  r*sin(  i   *dPhi ) , r*cos(  i   *dPhi )  , 0. ) ;
      Vector3D rv1(  r*sin( (i+1)*dPhi ) , r*cos( (i+1)*dPhi )  , 0. ) ;

      Vector3D pl0 =  zv + rv0 ;
      Vector3D pl1 =  zv + rv1 ;
      Vector3D pl2 = -zv + rv1  ;
      Vector3D pl3 = -zv + rv0 ;

      lines.push_back( std::make_pair( pl0, pl1 ) ) ;
      lines.push_back( std::make_pair( pl1, pl2 ) ) ;
      lines.push_back( std::make_pair( pl2, pl3 ) ) ;
      lines.push_back( std::make_pair( pl3, pl0 ) ) ;
    }
    return lines;
  }
};

class SimpleCylinder : public dd4hep::rec::VolSurface{
public:
  SimpleCylinder( dd4hep::Volume vol, dd4hep::rec::SurfaceType type, double thickness_inner ,
		  double thickness_outer,  Vector3D origin ) :
    dd4hep::rec::VolSurface( new SimpleCylinderImpl( vol,  type,  thickness_inner , thickness_outer, origin ) ) {
  }
  SimpleCylinderImpl* operator->() { return static_cast<SimpleCylinderImpl*>( _surf ) ; }
} ;


static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  printout(dd4hep::DEBUG,"Lhe_Beampipe", "Creating Beampipe" ) ;
  
//  typedef vector<PlacedVolume> Placements;
  xml_det_t  x_det = e;
  string     name  = x_det.nameStr();
  DetElement sdet (name,x_det.id());
  Material   mat  (description.material(x_det.materialStr()));
//  vector<double> rmin,rmax,z,thickness;
  double rmin, rmax, thickness, z;
//  PlacedVolume pv;

// multiplication factor for ellipse major radius
// rmin        ellipse short radius
// rmax        ellipse long radius
// thickness   BP thickness
// z           z-length
  rmax = 1.;
  thickness = 1.;
  rmin = 1.;

  for(xml_coll_t c(e,_U(zplane)); c; ++c )   {
    xml_comp_t dim(c);
    rmin = dim.rmin();
    rmax = dim.rmax();
    thickness = dim.thickness();
    z = dim.z();
//    rmin.push_back(dim.rmin());
//    rmax.push_back(dim.rmax());
//    z.push_back(dim.z());
//    thickness.push_back(dim.thickness());
  }

  double ra    = 1.;         // ellipse long radius init
         ra    = rmax;       // ellipse long radius
  double rb    = 1.;         // ellipse short radius init
         rb    = rmin;       // ellipse short radius
  double thick = 1.;  // BP thickness
         thick = thickness;  // BP thickness
 
  EllipticalTube bpElTubeOut(ra+thick, rb+thick, z);
  EllipticalTube bpElTubeInn(ra, rb, z+thick);
  SubtractionSolid bpElTube(bpElTubeOut,bpElTubeInn);

  Tube bpTube1(rb, rb+thick, z+thick, 3*M_PI/2, M_PI/2);
  UnionSolid beamTube1(bpElTube,bpTube1);

  Tube bpTube2(rb+thick, ra+thick, z+thick, 3*M_PI/2, M_PI/2);
  SubtractionSolid beamTube(beamTube1,bpTube2);
  
  Volume  det_vol(name, beamTube, mat);
  PlacedVolume pv = description.pickMotherVolume(sdet).placeVolume(det_vol,Position(0,0,0));
  
  double z_offset = x_det.hasAttr(_U(z_offset)) ? x_det.z_offset() : 0.0;
  bool    reflect = x_det.hasAttr(_U(reflect)) ? x_det.reflect() : false;

if ( z_offset >= 0 ) {
  if ( reflect) {
   PlacedVolume pv = description.pickMotherVolume(sdet).placeVolume(det_vol,Position(0,0,0));
   sdet.setPlacement(pv);
   det_vol.setVisAttributes(description, x_det.visStr());
   det_vol.setLimitSet(description, x_det.limitsStr());
   det_vol.setRegion(description, x_det.regionStr());
     if ( x_det.isSensitive() )   {
    SensitiveDetector sd = sens;
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    det_vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
    }
   } else {
   PlacedVolume pv = description.pickMotherVolume(sdet).placeVolume(det_vol,Position(0,0,z_offset));
   sdet.setPlacement(pv);
   det_vol.setVisAttributes(description, x_det.visStr());
   det_vol.setLimitSet(description, x_det.limitsStr());
   det_vol.setRegion(description, x_det.regionStr());
     if ( x_det.isSensitive() )   {
    SensitiveDetector sd = sens;
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    det_vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
    }
   }
  } else {
   PlacedVolume pv = description.pickMotherVolume(sdet).placeVolume(det_vol,Position(0,0,z_offset-z));
   sdet.setPlacement(pv);
   det_vol.setVisAttributes(description, x_det.visStr());
   det_vol.setLimitSet(description, x_det.limitsStr());
   det_vol.setRegion(description, x_det.regionStr());
     if ( x_det.isSensitive() )   {
    SensitiveDetector sd = sens;
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    det_vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
    }
  }
 
  if ( x_det.hasAttr(_U(id)) )  {
    int det_id = x_det.id();
    pv.addPhysVolID("system",det_id);
  }
  return sdet;
}

DECLARE_DETELEMENT(Lhe_BeamPipe_Central,create_detector)

