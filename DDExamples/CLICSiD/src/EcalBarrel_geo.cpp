// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include "TGeoTrd2.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  static double tolerance = 0e0;
  Layering      layering (e);
  xml_det_t     x_det     = e;
  Material      air       = lcdd.air();
  Material      vacuum    = lcdd.vacuum();
  int           det_id    = x_det.id();
  string        det_name  = x_det.nameStr();
  xml_comp_t    x_staves  = x_det.staves();
  xml_comp_t    x_dim     = x_det.dimensions();
  int           nsides    = x_dim.numsides();
  double        inner_r   = x_dim.rmin();
  double        dphi      = 2*M_PI / nsides;
  double        hphi      = dphi/2;
  double        mod_z     = layering.totalThickness();
  double        outer_r   = inner_r + mod_z;
  double        totThick  = mod_z;
  DetElement    sdet      (det_name,det_id);
  Volume        motherVol = lcdd.pickMotherVolume(sdet);
  PolyhedraRegular hedra  (nsides,inner_r,inner_r+totThick+tolerance*2e0,x_dim.z());
  Volume        envelope  (det_name+"_envelope",hedra,air);
  PlacedVolume  env_phv   = motherVol.placeVolume(envelope,Rotation(0,0,M_PI/nsides));

  env_phv.addPhysVolID("system",det_id);
  env_phv.addPhysVolID("barrel",0);
  sdet.setPlacement(env_phv);

  DetElement    stave_det("stave0",det_id);
  double dx = mod_z / std::sin(dphi); // dx per layer
  dx = 0;
    
  // Compute the top and bottom face measurements.
  double trd_x2 = (2 * std::tan(hphi) * outer_r - dx)/2 - tolerance;
  double trd_x1 = (2 * std::tan(hphi) * inner_r + dx)/2 - tolerance;
  double trd_y1 = x_dim.z()/2 - tolerance;
  double trd_y2 = trd_y1;
  double trd_z  = mod_z/2 - tolerance;
		
  // Create the trapezoid for the stave.
  Trapezoid trd(trd_x1, // Outer side, i.e. the "short" X side.
		trd_x2, // Inner side, i.e. the "long"  X side.
		trd_y1, // Corresponds to subdetector (or module) Z.
		trd_y2, //
		trd_z); // Thickness, in Y for top stave, when rotated.

  Volume mod_vol(det_name+"_module",trd,air);

  sens.setType("calorimeter");
  { // =====  buildBarrelStave(lcdd, sens, module_volume) =====
    // Parameters for computing the layer X dimension:
    double stave_z  = trd_y1/2;
    double l_dim_x  = trd_x1/2;                            // Starting X dimension for the layer.
    double adj      = (l_dim_x-trd_x2/2)/2;                // Adjacent angle of triangle.
    double hyp      = std::sqrt(trd_z*trd_z/4 + adj*adj);  // Hypotenuse of triangle.
    double beta     = std::acos(adj / hyp);                // Lower-right angle of triangle.
    double tan_beta = std::tan(beta);                      // Primary coefficient for figuring X.
    double l_pos_z  = -(layering.totalThickness() / 2);

    // Loop over the sets of layer elements in the detector.
    int l_num = 1;
    for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
      xml_comp_t x_layer = li;
      int repeat = x_layer.repeat();
      // Loop over number of repeats for this layer.
      for (int j=0; j<repeat; j++)    {
	string l_name = _toString(l_num,"layer%d");
	double l_thickness = layering.layer(l_num-1)->thickness();  // Layer's thickness.
	double xcut = (l_thickness / tan_beta);                     // X dimension for this layer.
	l_dim_x -= xcut/2;

	Position   l_pos(0,0,l_pos_z+l_thickness/2);      // Position of the layer.
	Box        l_box(l_dim_x*2-tolerance,stave_z*2-tolerance,l_thickness-tolerance);
	Volume     l_vol(det_name+"_"+l_name,l_box,air);
	DetElement layer(stave_det, l_name, det_id);

	// Loop over the sublayers or slices for this layer.
	int s_num = 1;
	double s_pos_z = -(l_thickness / 2);
	for(xml_coll_t si(x_layer,_U(slice)); si; ++si)  {
	  xml_comp_t x_slice = si;
	  string     s_name  =  _toString(s_num,"slice%d");
	  double     s_thick = x_slice.thickness();
	  Box        s_box(l_dim_x*2-tolerance,stave_z*2-tolerance,s_thick-tolerance);
	  Volume     s_vol(det_name+"_"+l_name+"_"+s_name,s_box,lcdd.material(x_slice.materialStr()));
          DetElement slice(layer,s_name,det_id);

          if ( x_slice.isSensitive() ) {
	    s_vol.setSensitiveDetector(sens);
	  }
          slice.setAttributes(lcdd,s_vol,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());

          // Slice placement.
          PlacedVolume slice_phv = l_vol.placeVolume(s_vol,Position(0,0,s_pos_z+s_thick/2));
          slice_phv.addPhysVolID("slice", s_num);
          slice.setPlacement(slice_phv);
          // Increment Z position of slice.
          s_pos_z += s_thick;
                                        
          // Increment slice number.
          ++s_num;
        }        

        // Set region, limitset, and vis of layer.
        layer.setAttributes(lcdd,l_vol,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());

        PlacedVolume layer_phv = mod_vol.placeVolume(l_vol,l_pos);
        layer_phv.addPhysVolID("layer", l_num);
        layer.setPlacement(layer_phv);
        // Increment to next layer Z position.
        l_pos_z += l_thickness;          
        ++l_num;
      }
    }
  }

  // Set stave visualization.
  if (x_staves)   {
    mod_vol.setVisAttributes(lcdd.visAttributes(x_staves.visStr()));
  }
  // Phi start for a stave.
  double phi = M_PI / nsides;
  double mod_x_off = dx / 2;             // Stave X offset, derived from the dx.
  double mod_y_off = inner_r + mod_z/2;  // Stave Y offset

  // Create nsides staves.
  for (int i = 0; i < nsides; i++, phi -= dphi)      { // i is module number
    // Compute the stave position
    double m_pos_x = mod_x_off * std::cos(phi) - mod_y_off * std::sin(phi);
    double m_pos_y = mod_x_off * std::sin(phi) + mod_y_off * std::cos(phi);
    Transform3D tr(Rotation(0,phi,M_PI*0.5),Translation3D(-m_pos_x,-m_pos_y,0));
    PlacedVolume pv = envelope.placeVolume(mod_vol,tr);
    pv.addPhysVolID("system",det_id);
    pv.addPhysVolID("barrel",0);
    pv.addPhysVolID("module",i+1);
    DetElement sd = i==0 ? stave_det : stave_det.clone(_toString(i,"stave%d"));
    sd.setPlacement(pv);
    sdet.add(sd);
  }

  // Set envelope volume attributes.
  envelope.setAttributes(lcdd,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
  return sdet;
}

DECLARE_DETELEMENT(EcalBarrel,create_detector);
