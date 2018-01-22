//====================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//  Modified   : E.Pilicer ( tube + elliptical pipe layout )
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, Ref_t)    {
  xml_det_t  x_det = e;
  string     name  = x_det.nameStr();
  DetElement sdet (name,x_det.id());
  Material   mat  (description.material(x_det.materialStr()));

  // multiplication factor for ellipse major radius
  double c0 = 3.5;
  double rmin = 0.0, rmax = 0.0, z = 0.0;

  for(xml_coll_t c(x_det,_U(zplane)); c; ++c)  {
    xml_comp_t dim(c);
    rmin = dim.rmin();
    rmax = dim.rmax();
    z    = dim.z();
  }
 
  double ra    = rmax * c0;      // elipse long radius
  double rb    = rmax;           // elipse short radius
  double thick = rmax - rmin;    // pipe wall thickness
 
  EllipticalTube bpElTubeOut(ra+thick, rb+thick, z);
  EllipticalTube bpElTubeInn(ra, rb, z+thick);
  SubtractionSolid bpElTube(bpElTubeOut,bpElTubeInn);

  Tube bpTube1(rb, rb+thick, z+thick, 3*M_PI/2, M_PI/2);
  UnionSolid beamTube1(bpElTube,bpTube1);

  Tube bpTube2(rb+thick, ra+thick, z+thick, 3*M_PI/2, M_PI/2);
  SubtractionSolid beamTube(beamTube1,bpTube2);
  
  Volume     volume(name, beamTube, mat);
  
  double z_offset = x_det.hasAttr(_U(z_offset)) ? x_det.z_offset() : 0.0;

  volume.setVisAttributes(description, x_det.visStr());
  PlacedVolume pv = description.pickMotherVolume(sdet).placeVolume(volume,Position(0,0,z_offset));
  sdet.setPlacement(pv);
  
  if ( x_det.hasAttr(_U(id)) )  {
    int det_id = x_det.id();
    pv.addPhysVolID("system",det_id);
  }
  return sdet;
}

DECLARE_DETELEMENT(LheD_tracker_BeamPipe,create_detector)

