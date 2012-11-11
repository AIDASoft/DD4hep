// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
// This subdetector is rather simple: Just a set of disk
// perpendicular to the beam.
// - No special user limits required for simulation.
// - 
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "TGeoTube.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
#define _U(text)  Unicode(#text)

/*
 *   Tesla namespace declaration
 */
namespace Tesla {
  
  /** @class DiskExtension Ftd01.h Tesla/Ftd01.h
   *
   *  Example how to use a detector element extension object
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct DiskExtension {
    PlacedVolume  sensitive, support;
    DiskExtension() {}
    DiskExtension(const DiskExtension& c) 
      : sensitive(c.sensitive), support(c.support)  {}
    DiskExtension(const DiskExtension& c, DetElement& /* org */) 
      : sensitive(c.sensitive), support(c.support)  {}
    DiskExtension& operator=(const DiskExtension& c) { 
      sensitive=c.sensitive;
      support=c.support;
      return *this;
    }
  };
}

static Ref_t create_element(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  Rotation    reflect_rot(M_PI,0,0);
  DetElement  sdet(name,x_det.id());
  Assembly    ftdVol(name);
  Volume      motherVol   = lcdd.pickMotherVolume(sdet);

  xml_comp_t  x_disks = x_det.child(_U(disks));
  xml_comp_t  x_cables = x_det.child(_U(cables));
  xml_comp_t  x_cylinder = x_det.child(_U(cylinder));
  xml_comp_t  x_support = x_det.child(_U(support));
  xml_comp_t  x_inner_support = x_support.child(_U(inner));
  xml_comp_t  x_outer_support = x_support.child(_U(outer));

  VisAttr  supportVis  = lcdd.visAttributes(x_support.visStr());
  VisAttr  cablesVis   = lcdd.visAttributes(x_cables.visStr());
  VisAttr  cylinderVis = lcdd.visAttributes(x_cylinder.visStr());
  Material cablesMat   = lcdd.material(x_cables.materialStr());
  Material supportMat  = lcdd.material(x_support.materialStr());
  Material diskMat     = lcdd.material(x_disks.materialStr());

  double si_thickness  = x_disks.attr<double>(_U(si_thickness1));
  double si_thickness2 = x_disks.attr<double>(_U(si_thickness2));
  double inner_support_thickness = x_inner_support.thickness();
  double inner_support_length    = x_inner_support.length();
  double outer_support_thickness = x_outer_support.thickness();
  double outer_support_length    = x_outer_support.length();

  struct Cylinder { double z_start, z_stop, r1, r2; } inner_cyl, outer_cyl;
  PlacedVolume pv;
  for(xml_coll_t c(x_disks,_U(disk)); c; ++c)  {
    typedef Tesla::DiskExtension _Ext;
    xml_comp_t x_disk = c;
    int id = x_disk.id();
    double z_pos = x_disk.z();
    double inner_r = x_disk.inner_r();
    double outer_r = x_disk.outer_r();
    DetElement disk_pos(sdet,_toString(id,"disk%d_neg"),id);
    DetElement disk_neg(sdet,_toString(id,"disk%d_pos"),id);
    _Ext *ext_pos=disk_pos.addExtension<_Ext>(new _Ext());
    _Ext *ext_neg=disk_neg.addExtension<_Ext>(new _Ext());
    { //... Si sensitive
      Tube   tub(inner_r,outer_r,si_thickness/2.0);
      Volume vol(_toString(id,"_disk%d_Si"),tub,diskMat);
      vol.setVisAttributes(lcdd.visAttributes(x_disk.visStr()));
      // This is the sensitive element: add senssitive detector
      vol.setSensitiveDetector(sens);
      pv = ftdVol.placeVolume(vol,Position(0,0, z_pos));
      pv.addPhysVolID("disk",id);
      disk_pos.setPlacement(pv);
      ext_pos->sensitive = pv;
      pv = ftdVol.placeVolume(vol,Position(0,0,-z_pos));
      pv.addPhysVolID("disk",-id);
      disk_neg.setPlacement(pv);
      ext_neg->sensitive = pv;
    }
    { //... Support
      Tube   tub(inner_r,outer_r,inner_support_thickness);
      Volume vol(_toString(id,"_disk%d_support"),tub,supportMat);
      double z = z_pos + si_thickness + inner_support_thickness;
      vol.setVisAttributes(supportVis);
      pv = ftdVol.placeVolume(vol,Position(0,0, z));
      pv.addPhysVolID("disk",id);
      ext_pos->support = pv;
      pv = ftdVol.placeVolume(vol,Position(0,0,-z));
      pv.addPhysVolID("disk",-id);
      ext_neg->support = pv;
    }
    /* { //... Outer support rings
      Tube   tub(outer_r,outer_r+outer_support.thickness,outer_support_length);
      Volume vol(_toString(id,"disk%d_outer_support"),tub,supportMat);
      vol.setVisAttributes(supportVis);
      pv = ftdVol.placeVolume(vol,Position(0,0, z_pos));
      pv.addPhysVolID("disk",id);
      pv = ftdVol.placeVolume(vol,Position(0,0,-z_pos));
      pv.addPhysVolID("disk",-id);
    } */

    switch(id) {
    case 1:
      //... keep information for the outer and inner cylinders
      inner_cyl.z_start = z_pos;
      //... safety margin due to slope in rz-plane
      inner_cyl.r1 = inner_r - 0.5;
      break;
    case 4:
      si_thickness =  si_thickness2; 
      inner_support_thickness = inner_support_thickness - 0.5;
      outer_cyl.z_start = z_pos;
      break;
    case 7:
      inner_cyl.z_stop = outer_cyl.z_stop = z_pos;
      //... +-0.5*mm - safety margin due to slope in rz-plane
      outer_cyl.r1 = outer_r + 0.5;
      inner_cyl.r2 = inner_r - 0.5;
      break;
    default:
      break;
    }
  }

  double outer_cyl_thickness = x_cylinder.thickness();
  double cables_thickness = x_cables.thickness();
  { //... Outer cylinder 
    double zhalf = (outer_cyl.z_stop-outer_cyl.z_start)/2.0;
    double cyl_z_pos = outer_cyl.z_start + zhalf;

    assert(outer_cyl.z_start>0);
    assert(outer_cyl.z_stop>0);
    assert(zhalf>0);
    
    Tube   tub_cyl(outer_cyl.r1,outer_cyl.r1+outer_cyl_thickness+cables_thickness,zhalf);
    Volume vol_cyl("outerCylinder",tub_cyl,supportMat);
    vol_cyl.setVisAttributes(cylinderVis);
    pv = ftdVol.placeVolume(vol_cyl,Position(0,0,cyl_z_pos));
    pv.addPhysVolID("side",1);
    pv = ftdVol.placeVolume(vol_cyl,Position(0,0,-cyl_z_pos));
    pv.addPhysVolID("side",-1);

    /*
    Tube   tub_cables(outer_cyl.r1,outer_cyl.r1+cables_thickness,zhalf);
    Volume vol_cables("cables",tub_cables,cablesMat);
    vol_cables.setVisAttributes(cablesVis);
    ftdVol.placeVolume(vol_cables,Position());
     */
  }
  { //... Inner cylinder (cone)
    double zhalf = (inner_cyl.z_stop-inner_cyl.z_start)/2.0;
    double cyl_z_pos = inner_cyl.z_start + zhalf;
    assert(inner_cyl.z_start>0);
    assert(inner_cyl.z_stop>0);
    assert(zhalf>0);
    
    Cone cone_cyl(inner_cyl.r1-outer_cyl_thickness-cables_thickness,inner_cyl.r1,
	      inner_cyl.r2-outer_cyl_thickness-cables_thickness,inner_cyl.r2,zhalf);
    Volume vol_cyl("innerCables",cone_cyl,supportMat);
    vol_cyl.setVisAttributes(cylinderVis);
    pv = ftdVol.placeVolume(vol_cyl,Position(0,0,cyl_z_pos));
    pv.addPhysVolID("side",1);
    pv = ftdVol.placeVolume(vol_cyl,Position(0,0,-cyl_z_pos));
    pv.addPhysVolID("side",-1);

    Cone cone_cables(inner_cyl.r1-cables_thickness,inner_cyl.r1,
		     inner_cyl.r2-cables_thickness,inner_cyl.r2,zhalf);
    Volume vol_cables("coneCables",cone_cables,cablesMat);
    vol_cables.setVisAttributes(cablesVis);
    ftdVol.placeVolume(vol_cables);
  }
  pv = motherVol.placeVolume(ftdVol);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_ftd01,create_element);
