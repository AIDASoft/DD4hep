// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  Rotation    reflect_rot(M_PI,0,0);
  DetElement  sdet(name,x_det.id());
  Assembly    maskVol(name);
  Volume      motherVol = lcdd.pickMotherVolume(sdet);
  xml_comp_t  x_masks = x_det.child(Unicode("masks"));
  xml_comp_t  x_quad  = x_det.child(Unicode("quadrupole"));
  double      z0, dz;

  PlacedVolume pv;
  for(xml_coll_t c(x_masks,Unicode("mask")); c; ++c)  {
    xml_comp_t x_m = c;
    int id = x_m.id();
    double z1 = x_m.zmin(),   z2 = x_m.zmax();
    double rmin = x_m.rmin(), rmax = x_m.rmax();
    dz = (z2 - z1)/2.0;
    z0 = (z1+z2)/2.0;

    Tube   tub(rmin,rmax,dz);
    Volume vol(_toString(id,"_mask%d"),tub,lcdd.material(x_m.materialStr()));
    vol.setVisAttributes(lcdd.visAttributes(x_m.visStr()));
    pv = maskVol.placeVolume(vol,Position(0,0,z0));
    pv.addPhysVolID("mask",id);
    pv = maskVol.placeVolume(vol,Position(0,0,-z0));
    pv.addPhysVolID("mask",-id);
  }

  xml_comp_t x_yoke = x_quad.child(Unicode("yoke"));
  xml_comp_t x_coil = x_quad.child(Unicode("coil"));
  double yoke_dz  = (x_quad.end()-x_yoke.z())/2.0;
  double yoke_pos = x_yoke.z() + yoke_dz;
  double coil_dz  = (x_quad.end()-x_coil.z())/2.0;
  double coil_pos = x_coil.z() + coil_dz;
  {  // Place yoke
    Tube  iron(x_yoke.rmin(),x_yoke.rmax(),yoke_dz);
    Tube  cave(x_coil.rmin(),x_coil.rmax(),coil_dz+10.0);
    SubtractionSolid yoke(iron,cave,Position(0,0,coil_pos - yoke_pos + 10.),Rotation());
    Volume vol("yoke",yoke,lcdd.material(x_yoke.materialStr()));
    vol.setVisAttributes(lcdd.visAttributes(x_yoke.visStr()));
    maskVol.placeVolume(vol,Position(0,0, yoke_pos));
    maskVol.placeVolume(vol,Position(0,0,-yoke_pos),reflect_rot);
  }
  {  // Place coil
    Tube   tub(x_coil.rmin(),x_coil.rmax(),coil_dz);
    Volume vol("coil",tub,lcdd.material(x_coil.materialStr()));
    vol.setVisAttributes(lcdd.visAttributes(x_coil.visStr()));
    maskVol.placeVolume(vol,Position(0,0, coil_pos));
    maskVol.placeVolume(vol,Position(0,0,-coil_pos),reflect_rot);
  }
  pv = motherVol.placeVolume(maskVol);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_mask04,create_element);
