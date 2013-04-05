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

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens_det)  {
  xml_det_t  x_det  = e;
  string     name   = x_det.nameStr();
  DetElement sdet(name,x_det.id());
  Assembly   assembly(name);
  xml_comp_t x_param      = x_det.child(_U(param));
  Material   sensitiveMat = lcdd.material("silicon_2.33gccm");
  Material   supportMat   = lcdd.material("Graphite");
  VisAttr    sensitiveVis = lcdd.visAttributes("SetSensitiveVis");
  VisAttr    supportVis   = lcdd.visAttributes("SetSupportVis");

  double TPC_outer_radius           = lcdd.constant<double>("TPC_outer_radius");
  double TPC_Ecal_Hcal_barrel_halfZ = lcdd.constant<double>("TPC_Ecal_Hcal_barrel_halfZ");
  double Ecal_Tpc_gap               = lcdd.constant<double>("Ecal_Tpc_gap");
  double ECal_min_r                 = TPC_outer_radius + Ecal_Tpc_gap;
  double sensitive_thickness        = x_param.attr<double>(_Unicode(sensitive_thickness));
  double support_thickness          = x_param.attr<double>(_Unicode(support_thickness));

#if 0
  //... The SET Sensitive detector: Threshold is 20% of a MIP. For Si we have 340 KeV/mm as MIP.
  theSETSD = new TRKSD00("SET", sensitive_thickness * mm * 340 * keV * 0.2);
  RegisterSensitiveDetector(theSETSD);
#endif
  // build outer layer first
  // radius defined by distance from Rmin of ECal Barrel
  double inner_radius2   = ECal_min_r - x_param.attr<double>(_Unicode(distance_set2_ecal_barrel)); 
  double support_radius2 = inner_radius2 + 0.5*sensitive_thickness + 0.5*support_thickness;
  // half length is the same a the TPC
  double half_z2 = TPC_Ecal_Hcal_barrel_halfZ;

  //... Sensitive Cylinders Si
  Tube   sensTub(inner_radius2, inner_radius2+sensitive_thickness, TPC_Ecal_Hcal_barrel_halfZ);
  Volume sensVol(name+"_outer_sensor",sensTub,sensitiveMat);
  sensVol.setVisAttributes(sensitiveVis);
  sensVol.setSensitiveDetector(sens_det);
  assembly.placeVolume(sensVol).addPhysVolID("sensor",2);

  //... SET2 support Cylinder
  Tube   suppTub(inner_radius2+sensitive_thickness,
		 inner_radius2+sensitive_thickness+support_thickness,
		 TPC_Ecal_Hcal_barrel_halfZ);
  Volume suppVol(name+"_outer_support",suppTub,supportMat);
  suppVol.setVisAttributes(supportVis);
  assembly.placeVolume(suppVol);

  // now build inner layer
  double inner_radius1 = inner_radius2 - x_param.attr<double>(_Unicode(set_layer_radial_diff)); 
  double support_radius1 = inner_radius1 + 0.5*sensitive_thickness + 0.5*support_thickness;

  //... Sensitive Cylinders Si
  sensTub = Tube(inner_radius1,inner_radius1+sensitive_thickness,TPC_Ecal_Hcal_barrel_halfZ);
  sensVol = Volume(name+"_inner_sensor",sensTub,sensitiveMat);
  sensVol.setVisAttributes(sensitiveVis);
  sensVol.setSensitiveDetector(sens_det);
  assembly.placeVolume(sensVol).addPhysVolID("sensor",1);
  
  //... SET1 support Cylinder
  suppTub = Tube(inner_radius1+sensitive_thickness,
		 inner_radius1+sensitive_thickness+support_thickness,
		 TPC_Ecal_Hcal_barrel_halfZ);
  suppVol = Volume(name+"_inner_support",suppTub,supportMat);
  suppVol.setVisAttributes(supportVis);
  assembly.placeVolume(suppVol);

  // now place the full assembly
  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  PlacedVolume pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_SSet02,create_element);
