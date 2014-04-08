// $Id: Module_geo.cpp 784 2013-09-19 20:05:24Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for Testing purposes
//--------------------------------------------------------------------
//
//  Author     : C.Hombach, A. Karachaliou
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"


using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens) {
  xml_det_t x_det = e;
 	string det_name = x_det.nameStr();
 	string det_type = x_det.typeStr();
 	Material air = lcdd.air();
  Assembly assembly (det_name+"_assembly");
 	DetElement sdet(det_name,x_det.id());
  Volume motherVol = lcdd.pickMotherVolume(sdet);

  int m_id=0;
  
  PlacedVolume pv;

  for(xml_coll_t mi(x_det,_U(module)); mi; ++mi, ++m_id) {
    xml_comp_t x_mod = mi;
    string m_nam = x_mod.nameStr();
    double x = x_mod.x();
    double y = x_mod.y();
    double z = x_mod.z();
    Volume m_volume(det_name+"_"+m_nam, Box(10, 10, 1), air);
    m_volume.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
    pv = motherVol.placeVolume(m_volume,Transform3D(RotationZYX(0,0,0),Position(x,y,z)));
  }
  sdet.setPlacement(pv);
  return sdet;
}
DECLARE_DETELEMENT(TestBox,create_detector);
