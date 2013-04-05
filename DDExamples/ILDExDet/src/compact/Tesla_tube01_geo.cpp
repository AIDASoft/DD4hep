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
  struct Tube_Desc {  double zhalf, thickness, radius;  };
  struct Cone_Desc {  double z, rmin, rmax; };

  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  Rotation    reflect_rot(0,M_PI,0);
  DetElement  beampipe(name,x_det.id());
  Assembly    beampipeVol("assembly");
  Volume      motherVol   = lcdd.pickMotherVolume(beampipe);
  xml_comp_t  x_central   = x_det.child(_Unicode(central_tube));
  xml_comp_t  x_lateral   = x_det.child(_Unicode(lateral_tubes));
  xml_comp_t  x_ends      = x_det.child(_Unicode(ends));
  xml_comp_t  x_vacuum    = x_det.child(_Unicode(vacuum));
  Material    beampipeMat = lcdd.material(xml_comp_t(x_det).materialStr());
  Material    ironMat     = lcdd.material("Iron");
  Material    vacuumMat   = lcdd.vacuum();
  VisAttr     centralVis  = lcdd.visAttributes(x_central.visStr());
  VisAttr     beampipeVis = lcdd.visAttributes(x_det.visStr());
  VisAttr     vacuumVis   = lcdd.visAttributes(x_vacuum.visStr());
  VisAttr     endVis      = lcdd.visAttributes(x_ends.visStr());
  Tube_Desc   central     = {x_central.zhalf(),x_central.thickness(),x_central.inner_r()};
  Tube_Desc   lateral     = {x_lateral.zmin(), x_lateral.thickness(),x_lateral.inner_r()};
  PlacedVolume pv;

  { // beam vacuum inside the tube  
    Tube   tube(0,central.radius,central.zhalf);
    Volume vol (name+"_vacuum",tube,vacuumMat);
    vol.setVisAttributes(vacuumVis);
    beampipeVol.placeVolume(vol);
  }
  { // Be tube
    Tube   tube(central.radius,central.radius+central.thickness,central.zhalf);
    Volume vol (name+"_Be_tube",tube,beampipeMat);
    vol.setVisAttributes(beampipeVis);
    beampipeVol.placeVolume(vol);
  }

  // Lateral cones and tubes
  double inner_zmin      = x_lateral.zmin();
  double end_zmin        = x_ends.zmin();
  //double end_zmax      = x_ends.zmax();
  double end_thickness   = x_ends.thickness();
  double inner_zhalf     = (end_zmin+inner_zmin)/2.0;

  // lateral cones
  double cone_half_z = (inner_zmin - central.zhalf)/2.0;
  assert (cone_half_z>0);

  double z_cone = (inner_zmin + central.zhalf)/2.0;
  { // Beam vacuum inside cones
    Cone   cone(cone_half_z,0,central.radius,0,lateral.radius);
    Volume vol (name+"_lateral",cone,vacuumMat);
    vol.setVisAttributes(vacuumVis);
    beampipeVol.placeVolume(vol,Position(0,0, z_cone));
    beampipeVol.placeVolume(vol,Position(0,0,-z_cone),reflect_rot);
  }
  {  // Be cones
    Cone cone(cone_half_z,
	      central.radius,central.radius+central.thickness,
	      lateral.radius,lateral.radius+lateral.thickness);
    Volume vol(name+"_BeCone",cone,beampipeMat);
    vol.setVisAttributes(beampipeVis);
    beampipeVol.placeVolume(vol,Position(0,0, z_cone));
    beampipeVol.placeVolume(vol,Position(0,0,-z_cone),reflect_rot);
  }

  // inner lateral tubes
  double z_tube = (end_zmin + inner_zmin)/2.0;
  { // inner beam vacuum lateral tubes
    Tube   tube(0,lateral.radius,inner_zhalf);
    Volume vol (name+"_lateral_vacuum",tube,vacuumMat);
    vol.setVisAttributes(vacuumVis);
    beampipeVol.placeVolume(vol,Position(0,0, z_tube));
    beampipeVol.placeVolume(vol,Position(0,0,-z_tube));
  }

  { // inner Be lateral tubes
    Tube   tube(lateral.radius,lateral.radius+lateral.thickness,inner_zhalf);
    Volume vol (name+"_lateral_BeTube",tube, beampipeMat);
    vol.setVisAttributes(beampipeVis);
    beampipeVol.placeVolume(vol,Position(0,0, z_tube));
    beampipeVol.placeVolume(vol,Position(0,0,-z_tube));
  }

  { // VXD strip lines on lateral tubes
    xml_comp_t s = x_det.child(_Unicode(strips));
    Tube  tube(lateral.radius+lateral.thickness+s.gap(),
               lateral.radius+lateral.thickness+s.gap()+s.thickness(),inner_zhalf);
    Volume vol(name+"_lateral_Strips",tube,lcdd.material(s.materialStr()));
    vol.setVisAttributes(lcdd.visAttributes(s.visStr()));
    beampipeVol.placeVolume(vol,Position(0,0, z_tube));
    beampipeVol.placeVolume(vol,Position(0,0,-z_tube));
  }

  // ends lateral cones and tubes
  size_t cnt = 0;
  Cone_Desc cones[5];
  for(xml_coll_t c(x_lateral,_U(cone)); c; ++c, ++cnt)  {
    xml_comp_t x_con = c;
    cones[cnt].z     = x_con.z();
    cones[cnt].rmin  = x_con.rmin();
    cones[cnt].rmax  = x_con.rmax();
  }
  { // vacuum lateral cone
    double dz = (x_lateral.dz() - end_thickness)/2.0;
    Position pos, neg;

    Cone   vac_end_cone(dz,0.,lateral.radius,0.,cones[0].rmin);
    Volume vac_end_vol (name+"_lat_vac",vac_end_cone,vacuumMat);
    vac_end_vol.setVisAttributes(vacuumVis);

    Cone   fe_end_cone (dz,lateral.radius,lateral.radius+end_thickness,
			cones[0].rmin,cones[0].rmin+end_thickness);
    Volume fe_end_vol(name+"_lateral_fe",fe_end_cone,ironMat);
    fe_end_vol.setVisAttributes(endVis);

    // put vacuum and iron cones into world
    pos.SetZ(inner_zhalf*2 + dz);
    neg.SetZ(-pos.Z());
    beampipeVol.placeVolume(fe_end_vol,pos);
    beampipeVol.placeVolume(fe_end_vol,neg,reflect_rot);
    beampipeVol.placeVolume(vac_end_vol,pos);
    beampipeVol.placeVolume(vac_end_vol,neg,reflect_rot);

    // lateral tubes Fe and vacuum
    pos.SetZ(z_cone+dz);
    neg.SetZ(-pos.Z());
    for(int i=0; i<5; ++i )  {
      Tube   vac_tub(0, cones[i].rmin, cones[i].z);
      Volume vac_vol(_toString(i,"vac_%d"),vac_tub,vacuumMat);
      vac_vol.setVisAttributes(vacuumVis);
      // Update Z-placement
      pos.SetZ(pos.Z() + cones[i].z);
      neg.SetZ(-pos.Z());
      beampipeVol.placeVolume(vac_vol,pos);
      beampipeVol.placeVolume(vac_vol,neg);

      Tube   fe_tub (cones[i].rmin, cones[i].rmax, cones[i].z);
      Volume fe_vol (name+_toString(i,"_feTube%d"),fe_tub,ironMat);
      fe_vol.setVisAttributes(endVis);
      beampipeVol.placeVolume(fe_vol,pos);
      beampipeVol.placeVolume(fe_vol,neg);
      // Update Z-placement
      pos.SetZ(pos.Z() + cones[i].z);
      neg.SetZ(-pos.Z());
    }
  }     
  pv = motherVol.placeVolume(beampipeVol);
  beampipe.setPlacement(pv);
  return beampipe;
}

DECLARE_DETELEMENT(Tesla_tube01,create_element);
