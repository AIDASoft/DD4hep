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

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  struct Layer { double thickness, radius; Material mat; VisAttr vis; };
  xml_det_t    x_det  = e;
  string       name   = x_det.nameStr();
  DetElement   sdet(name,x_det.id());
  Assembly     assembly(name+"_assembly");
  xml_comp_t   x_par = x_det.child("params");
  double   TPC_Ecal_Hcal_barrel_halfZ = lcdd.constant<double>("TPC_Ecal_Hcal_barrel_halfZ");
  double   VXD_outer_radius           = lcdd.constant<double>("VXD_outer_radius");
  double   TPC_inner_radius           = lcdd.constant<double>("TPC_inner_radius");
  double   sit1_sit2_relative_gap     = x_par.attr<double>(_Unicode(sit1_sit2_relative_gap));
  double   sit2_tpc_gap               = x_par.attr<double>(_Unicode(sit2_tpc_gap));
  Layer    sensitive = { x_par.attr<double>(_Unicode(sensitive_thickness)),0,
			 lcdd.material("silicon_2.33gccm"),
			 lcdd.visAttributes("SITSensitiveVis")  };
  Layer    support   = { x_par.attr<double>(_Unicode(support_thickness)),0,
			 lcdd.material("Graphite"),
			 lcdd.visAttributes("SITSupportVis")  };

#if 0
  //... The SIT Sensitive detector
  // Threshold is 20% of a MIP. For Si we have 340 KeV/mm as MIP.
  theSITSD = new TRKSD00("SIT", sensitive.thickness * mm * 340 * keV* 0.2);
  RegisterSensitiveDetector(theSITSD);
#endif
  for(xml_coll_t c(x_det.child(_U(layers)),_U(layer)); c; ++c)  {
    xml_comp_t x_layer(c);
    int    id = x_layer.id();
    string layer_nam = name+_toString(id,"_layer%d");
    DetElement layer_det(sdet,layer_nam,id);
    double half_z=0, relative_half_z = x_layer.attr<double>(_U(half_z));

    if ( id == 1 )   {
      sensitive.radius = VXD_outer_radius + sit1_sit2_relative_gap*(TPC_inner_radius-sit2_tpc_gap-VXD_outer_radius);
      half_z = TPC_Ecal_Hcal_barrel_halfZ * relative_half_z;
    }
    else if( id == 2 )   {
      sensitive.radius = TPC_inner_radius - sit2_tpc_gap;
      half_z = TPC_Ecal_Hcal_barrel_halfZ * relative_half_z;
    }
    else {
      throw runtime_error("Invalid layer number for SSit03.");
    }
    support.radius = sensitive.radius + 0.5*sensitive.thickness + 0.5*support.thickness;

    assert((sensitive.radius - 0.5*sensitive.thickness)>VXD_outer_radius);
    assert((support.radius + 0.5*support.thickness)<TPC_inner_radius);

    //... Sensitive Cylinders Si
    Tube   siTube(sensitive.radius-0.5*sensitive.thickness,sensitive.radius + 0.5*sensitive.thickness,half_z);
    Volume siVol (layer_nam+"_tube",siTube,sensitive.mat);
    siVol.setVisAttributes(sensitive.vis);
    siVol.setSensitiveDetector(sens);
    assembly.placeVolume(siVol).addPhysVolID("layer",id);

    //... Sit support Cylinder
    Tube   suppTube(support.radius - 0.5*support.thickness,support.radius + 0.5*support.thickness,half_z);
    Volume suppVol(layer_nam+"_support",suppTube,support.mat);
    suppVol.setVisAttributes(support.vis);
    assembly.placeVolume(suppVol).addPhysVolID("layer",0);
    layer_det.setPlacement(assembly.placeVolume(suppVol));
  }
  assembly.setVisAttributes(lcdd.visAttributes(x_det.visStr()));
  PlacedVolume pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Tesla_SSit03,create_element);
