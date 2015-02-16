// $Id: LhePolyconeSupport_geo.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  mod.:        P.Kostka LHeD (asymmetrical detector placement in z)
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, Ref_t)    {
  // XML detector object: DDCore/XML/XMLDetector.h
  xml_det_t  x_det = e;
  //Create the DetElement for DD4hep
  string     name  = x_det.nameStr();
  DetElement sdet (name,x_det.id());
  //DetElement d_det(x_det.nameStr(),x_det.id());
  
  Material   mat  (lcdd.material(x_det.materialStr()));

  // XML dimension object: DDCore/XML/XMLDimension.h   
  double ra    = 10.;     // elipse short radius
  double rb    = 2.2;     // elipse long radius
  double z     = 50.0;    // pipe length
  double thick = 0.45;    // pipe wall thickness
  double phi   = M_PI/2;  // rotation
 
  EllipticalTube bpElTubeOut(ra+thick, rb+thick, z);
  EllipticalTube bpElTubeInn(ra, rb, z+thick);
  SubtractionSolid bpElTube(bpElTubeOut,bpElTubeInn);

  Tube bpTube1(rb, rb+thick, z+thick, 3*M_PI/2, M_PI/2);
  UnionSolid beamTube1(bpElTube,bpTube1);

  Tube bpTube2(rb+thick, ra+thick, z+thick, 3*M_PI/2, M_PI/2);
  SubtractionSolid beamTube(beamTube1,bpTube2);
  
  Volume     volume(name, beamTube, mat);
  
  double z_offset = x_det.hasAttr(_U(z_offset)) ? x_det.z_offset() : 0.0;

  volume.setVisAttributes(lcdd, x_det.visStr());
  PlacedVolume pv = lcdd.pickMotherVolume(sdet).placeVolume(volume,Position(0,0,z_offset));
  sdet.setPlacement(pv);
  
  if ( x_det.hasAttr(_U(id)) )  {
    int det_id = x_det.id();
    pv.addPhysVolID("system",det_id);
  }
  return sdet;
}

DECLARE_DETELEMENT(LheD_tracker_BeamPipe,create_detector)

