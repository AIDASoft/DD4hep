// $Id$
//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
//
//  Generic cylindric shell detector to be used to measure 
//  e.g. escape energy from calorimeters.
//
//  Author     : M.Frank
//
//====================================================================

/*
  Example XML:

  <detector id="2" name="ContainmentShell" type="ZylinderShell" vis="VisibleRed" readout="ContainmentHits" >
      <comment>Containment shell to measure calorimeter escapes</comment>
      <material name="Air"/>
      <module name="Barrel" id="0" vis="VisibleRed">
        <zplane rmin="HcalBarrel_rmax+20*cm" rmax="HcalBarrel_rmax+22*cm" z="-2*HcalBarrel_zmax"/>
        <zplane rmin="HcalBarrel_rmax+20*cm" rmax="HcalBarrel_rmax+22*cm" z="2*HcalBarrel_zmax"/>
      </module>
      <module name="SideA" id="1" vis="VisibleRed">
        <zplane rmin="0" rmax="HcalBarrel_rmax+22*cm" z="2*HcalBarrel_zmax+10*cm"/>
        <zplane rmin="0" rmax="HcalBarrel_rmax+22*cm" z="2*HcalBarrel_zmax+20*cm"/>
      </module>
      <module name="SideB" id="2" vis="VisibleRed">
        <zplane rmin="0" rmax="HcalBarrel_rmax+22*cm" z="-(2*HcalBarrel_zmax+10*cm)"/>
        <zplane rmin="0" rmax="HcalBarrel_rmax+22*cm" z="-(2*HcalBarrel_zmax+20*cm)"/>
      </module>
    </detector>
  </detectors>

to be joined by a sensitive detector:
  <readouts>
    <readout name="ContainmentHits">
      <id>system:8,barrel:3</id>
    </readout>
  </readouts>


*/
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sensitive)  {
  xml_det_t  x_det   = e;
  string     name    = x_det.nameStr();
  DetElement sdet   (name,x_det.id());
  Assembly   assembly(name+"_assembly");
  Material   mat    (description.material(x_det.materialStr()));
  PlacedVolume pv;

  sensitive.setType("escape_counter");
  for(xml_coll_t m(e,_U(module)); m; ++m)  {
    xml_comp_t mod = m;
    vector<double> rmin,rmax,z;
    string vis = mod.visStr().empty() ? x_det.visStr() : mod.visStr();
    int num = 0;
    for(xml_coll_t c(m,_U(zplane)); c; ++c, ++num)  {
      xml_comp_t dim(c);
      rmin.push_back(dim.rmin());
      rmax.push_back(dim.rmax());
      z.push_back(dim.z()/2);
    }
    if ( num < 2 )  {
      throw runtime_error("ZylinderShell["+name+"]> Not enough Z planes. minimum is 2!");
    }
    Polycone   cone  (0.,2*M_PI,rmin,rmax,z);
    Volume     volume(name, cone, mat);
    volume.setVisAttributes(description, vis);
    volume.setSensitiveDetector(sensitive);
    pv = assembly.placeVolume(volume);
    pv.addPhysVolID("barrel",mod.id());
  }

  pv = description.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system",x_det.id());
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_CylinderShell,create_detector)
