// $Id:$
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

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens_det)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  DetElement  sdet(name,x_det.id());
  Assembly    assembly(name);
  xml_comp_t  x_param      = x_det.child(_U(param));
  Material    sensitiveMat = lcdd.material("silicon_2.33gccm");
  Material    supportMat   = lcdd.material("Graphite");
  VisAttr     sensitiveVis = lcdd.visAttributes("SetSensitiveVis");
  VisAttr     supportVis   = lcdd.visAttributes("SetSupportVis");
  double TPC_outer_radius            = lcdd.constant<double>("TPC_outer_radius");
  double Ecal_endcap_zmin            = lcdd.constant<double>("Ecal_endcap_zmin");
  double ECal_endcap_center_box_size = lcdd.constant<double>("Ecal_endcap_center_box_size");
  double ECal_EndCap_Plug_MaxR       = sqrt(2*ECal_endcap_center_box_size * ECal_endcap_center_box_size)/2;
  double Etd3_ECalEndCap_distance_z  = x_param.attr<double>("etd3_ecalendcap_distance_z");
  double layer_separation_z          = x_param.attr<double>("layer_separation_z");
  double sensitive_thickness         = x_param.attr<double>("sensitive_thickness");
  double support_thickness           = x_param.attr<double>("support_thickness");

  // inner radius defined radial clearance between the ETD  and ECal-EndCap Plug
  double inner_radius = ECal_EndCap_Plug_MaxR - x_param.attr<double>("etd_ecalplug_radial_clearance");
  // outer radius defined by radial difference to the TPC outer radius
  double outer_radius = TPC_outer_radius +  x_param.attr<double>("etd_tpcOuterR_radial_diff");

#if 0
  //... The ETD Sensitive Detector: Threshold is 20% of a MIP. For Si we have 340 KeV/mm as MIP.
  theETDSD =  new TRKSD00("ETD", sensitive_thickness * mm * 340 * keV * 0.2);
  RegisterSensitiveDetector(theETDSD);
#endif

  // build the disk volumes: Support
  Tube   suppTub(inner_radius, outer_radius, 0.5*support_thickness);
  Volume suppVol(name+"_support",suppTub,supportMat);
  suppVol.setVisAttributes(supportVis);
  
  //... Sensitive layer (Si)
  Tube   sensTub(inner_radius, outer_radius, 0.5*sensitive_thickness);
  Volume sensVol(name+"_sensor",sensTub,sensitiveMat);
  sensVol.setVisAttributes(sensitiveVis);
  sensVol.setSensitiveDetector(sens_det);

  // place support disk furthest from the IP first, then build the others from that
  for(int i = 0; i<3; ++i) {
    double z_pos = Ecal_endcap_zmin - Etd3_ECalEndCap_distance_z - (i*layer_separation_z);
    int disk_number = 3-i;
    // +z side, first sensitive, then support
    assembly.placeVolume(sensVol,Position(0,0,z_pos)).addPhysVolID("disk",disk_number);
    assembly.placeVolume(suppVol,Position(0,0,z_pos+sensitive_thickness/2+support_thickness/2));
    // -z side, first sensitive, then support
    assembly.placeVolume(sensVol,Position(0,0,-z_pos)).addPhysVolID("disk",-disk_number);
    assembly.placeVolume(suppVol,Position(0,0,-z_pos-sensitive_thickness/2-support_thickness/2));
  }

  // now place the full assembly
  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  PlacedVolume pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_SEtd02,create_element);
