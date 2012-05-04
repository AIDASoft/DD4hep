// $Id:$
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

static Ref_t create_detector(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)   {
  xml_det_t  x_det     = e;
  xml_dim_t  dim       = x_det.dimensions();
  Material   air       = lcdd.air();
  string     det_name  = x_det.nameStr();
  string     det_type  = x_det.typeStr();
  Tube       envelope   (lcdd,det_name);
  Volume     envelopeVol(lcdd,det_name,envelope,air);
  DetElement sdet       (det_name,det_type,x_det.id());
  double     z         = dim.outer_z();
  double     rmin      = dim.inner_r();
  double     r         = rmin;
  int        n         = 0;
    
  for(xml_coll_t c(x_det,_X(layer)); c; ++c)  {
    xml_comp_t x_layer = c;
    for(int i=0, m=0, repeat=x_layer.repeat(); i<repeat; ++i, m=0)  {
      string layer_name = det_name + _toString(n,"_layer%d");
      Tube   layer_tub(lcdd,layer_name);
      Volume layer_vol(lcdd,layer_name,layer_tub,air);
      double rlayer = r;
        
      for(xml_coll_t l(x_layer,_X(slice)); l; ++l, ++m)  {
	xml_comp_t x_slice = l;
	Material   slice_mat  = lcdd.material(x_slice.materialStr());
	string     slice_name = layer_name + _toString(m,"slice%d");
	Tube       slice_tube(lcdd,slice_name);
	Volume     slice_vol (lcdd,slice_name,slice_tube,slice_mat);
	double     router = r + x_slice.thickness();
          
	if ( x_slice.isSensitive() ) slice_vol.setSensitiveDetector(sens);
	slice_tube.setDimensions(r,router,z * 2);
	r = router;
	slice_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
	// Instantiate physical volume
	layer_vol.placeVolume(slice_vol,IdentityPos());
      }
      layer_vol.setVisAttributes(lcdd,x_layer.visStr());
      layer_tub.setDimensions(rlayer,r,z * 2);
        
      PlacedVolume layer_physvol = envelopeVol.placeVolume(layer_vol,IdentityPos());
      layer_physvol.addPhysVolID(_A(layer),n);
      ++n;
    }
  }
  envelope.setDimensions(rmin,r,2.*z);
  // Set region of slice
  envelopeVol.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
    
  PlacedVolume physvol = lcdd.pickMotherVolume(sdet).placeVolume(envelopeVol,IdentityPos());
  physvol.addPhysVolID(_A(system),sdet.id()).addPhysVolID(_A(barrel),0);
  sdet.addPlacement(physvol);
  return sdet;
}

DECLARE_DETELEMENT(CylindricalBarrelCalorimeter,create_detector);
