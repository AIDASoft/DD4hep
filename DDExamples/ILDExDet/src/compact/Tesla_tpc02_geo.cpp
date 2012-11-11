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

static Ref_t create_element(LCDD& lcdd, const xml_h& e)  {
  xml_det_t   x_det  = e;
  string      name   = x_det.nameStr();
  DetElement  sdet(name,x_det.id());
  Volume      motherVol   = lcdd.pickMotherVolume(sdet);
  xml_comp_t  x_envelope  = x_det.child(Unicode("envelope"));
  xml_comp_t  x_endcap    = x_det.child(Unicode("endplate"));
  xml_comp_t  x_inner     = x_det.child(Unicode("inner_wall"));
  xml_comp_t  x_outer     = x_det.child(Unicode("outer_wall"));
  xml_comp_t  x_sens      = x_det.child(Unicode("sensitive"));
  xml_comp_t  x_fch       = x_det.child(Unicode("fch"));
  Material    gasMat      = lcdd.material(x_sens.materialStr());
  PlacedVolume pv;

  struct cylinder_t { double inner, outer, zhalf; };
  cylinder_t env  = { x_envelope.inner_r(),  x_envelope.outer_r(),  x_envelope.zhalf() };
  cylinder_t wall = { x_inner.thickness(),   x_outer.thickness(),   x_endcap.thickness() };
  cylinder_t fch  = { x_fch.inner_r(),       x_fch.outer_r(),       x_fch.thickness()/2.0 };
  cylinder_t sens = { x_sens.inner_r(),      x_sens.outer_r(), 0.0 };

  // TPC sensitive detector
  SensitiveDetector sens_det("TPC");
  Readout ro = lcdd.readout(x_sens.readoutStr());
  sens_det.setHitsCollection(ro.name());
  sens_det.setReadout(ro);
  lcdd.addSensitiveDetector(sens_det);

  // the TPC mother volume
  Tube    envTub(env.inner,env.outer,env.zhalf);
  Volume  envVol(name+"_envelope",envTub,lcdd.air());
  envVol.setVisAttributes(lcdd.visAttributes(x_det.visStr()));

  // TPC Al inner shield 
  Tube    innerTub(env.inner,env.inner+wall.inner,env.zhalf);
  Volume  innerVol(name+"_inner",innerTub,lcdd.material(x_inner.materialStr()));
  innerVol.setVisAttributes(lcdd.visAttributes(x_inner.visStr()));
  envVol.placeVolume(innerVol);

  // TPC Al outer shield 
  Tube    outerTub(env.outer-wall.outer,env.outer,env.zhalf);
  Volume  outerVol(name+"_outer",outerTub,lcdd.material(x_outer.materialStr()));
  outerVol.setVisAttributes(lcdd.visAttributes(x_outer.visStr()));
  envVol.placeVolume(outerVol);

  // TPC gas chamber envelope
  Tube    gasTub(env.inner,env.outer+wall.outer,env.zhalf);
  Volume  gasVol(name+"_chamber",gasTub,gasMat);
  gasVol.setVisAttributes(lcdd.visAttributes(x_sens.visStr()));
  envVol.placeVolume(gasVol);
  
  // Now, the gas layers to stop steps on the boundaries
  VisAttr layerVis = lcdd.visAttributes("TPCLayerVis");
  for (int i=0, num_layer=x_sens.number(); i<num_layer; i++)   {
    double lay_thick = (sens.outer-sens.inner)/num_layer;
    Tube   layerTub(sens.inner+i*lay_thick,sens.inner+(i+1)*lay_thick,env.zhalf);
    Volume layerVol(name+_toString(i,"_layer%d"),layerTub,gasMat);      
    layerVol.setSensitiveDetector(sens_det);
    layerVol.setVisAttributes(layerVis);
    envVol.placeVolume(layerVol);
  }

  // TPC endplates
  Material endcapMat = lcdd.material(x_endcap.materialStr());
  //TGeoMixture* mix = dynamic_cast<TGeoMixture*>(endcapMat.ptr());
  TGeoMaterial* mix = dynamic_cast<TGeoMaterial*>(endcapMat.ptr()->GetMaterial());
  // Material = mix. Desity is mass for a box with 1 mm**2 as base and 100 mm long
  double density = (endcapMat.ptr()->GetMaterial())->GetDensity();
  //density = mix->GetWmixt()/(100*1);
  //endcapMat->SetDensity(density);
  Tube   endcapTub(env.inner,env.outer,wall.zhalf);
  Volume endcapVol(name+"_endcap",endcapTub,endcapMat);
  endcapVol.setVisAttributes(lcdd.visAttributes(x_endcap.visStr()));
  envVol.placeVolume(endcapVol,Position(0,0,  env.zhalf+wall.zhalf/2.0));
  envVol.placeVolume(endcapVol,Position(0,0,-(env.zhalf+wall.zhalf/2.0)));

  // FCH = two sensitive twin Si plates, just to register the particle step inside it.
  // Threshold is 20% of a MIP. For Si we have 0.34 KeV/micron as MIP.
  SensitiveDetector fchSD("TPCfch");
  ro = lcdd.readout(x_fch.readoutStr());
  fchSD.setHitsCollection(ro.name());
  fchSD.setReadout(ro);
  lcdd.addSensitiveDetector(fchSD);

  Tube   fchTub(fch.inner,fch.outer,fch.zhalf);
  Volume fchVol("fch",fchTub,lcdd.material(x_fch.materialStr()));
  fchVol.setVisAttributes(lcdd.visAttributes(x_fch.visStr()));
  fchVol.setSensitiveDetector(fchSD);
  envVol.placeVolume(fchVol,Position(0,0,  env.zhalf+wall.zhalf+fch.zhalf));
  envVol.placeVolume(fchVol,Position(0,0,-(env.zhalf+wall.zhalf+fch.zhalf)));

  pv = motherVol.placeVolume(envVol);
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_SUBDETECTOR(Tesla_tpc02,create_element);
