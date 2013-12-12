// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)   {
  xml_det_t  x_det     = e;
  xml_dim_t  dim       = x_det.dimensions();
  Material   air       = lcdd.air();
  string     det_name  = x_det.nameStr();
  DetElement sdet       (det_name,x_det.id());
  double     z         = dim.outer_z();
  double     rmin      = dim.inner_r();
  double     r         = rmin;
  int        n         = 0;
  Tube       envelope(rmin,2*rmin,2*z);
  Volume     envelopeVol(det_name+"_envelope",envelope,air);
    
  for(xml_coll_t c(x_det,_U(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
      string layer_name = det_name + _toString(n,"_layer%d");
      double rlayer = r;
      Tube   layer_tub(rmin,rlayer,2*z);
      Volume layer_vol(layer_name,layer_tub,air);
        
      for(xml_coll_t l(x_layer,_U(slice)); l; ++l, ++m)  {
	xml_comp_t x_slice = l;
	double     router = r + x_slice.thickness();
	Material   slice_mat  = lcdd.material(x_slice.materialStr());
	string     slice_name = layer_name + _toString(m,"slice%d");
	Tube       slice_tube(r,router,z*2);
	Volume     slice_vol (slice_name,slice_tube,slice_mat);
          
	if ( x_slice.isSensitive() ) {
	  sens.setType("calorimeter");
	  slice_vol.setSensitiveDetector(sens);
	}
	r = router;
	slice_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
	// Instantiate physical volume
	layer_vol.placeVolume(slice_vol);
      }
      layer_vol.setVisAttributes(lcdd,x_layer.visStr());
      layer_tub.setDimensions(rlayer,r,z*2,0,2*M_PI);
        
      PlacedVolume layer_physvol = envelopeVol.placeVolume(layer_vol);
      layer_physvol.addPhysVolID("layer",n);
      ++n;
    }
  }
  envelope.setDimensions(rmin,r,2*z);
  // Set region of slice
  envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
    
  PlacedVolume physvol = lcdd.pickMotherVolume(sdet).placeVolume(envelopeVol);
  physvol.addPhysVolID("system",sdet.id()).addPhysVolID(_U(barrel),0);
  sdet.setPlacement(physvol);
  return sdet;
}

DECLARE_DETELEMENT(CylindricalBarrelCalorimeter,create_detector);
