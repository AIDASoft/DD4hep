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

static Ref_t create_detector(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
  xml_det_t   x_det     = e;
  Material    air       = lcdd.air();
  int         det_id    = x_det.id();
  string      det_name  = x_det.nameStr();
  string      det_type  = x_det.typeStr();
  DetElement  sdet       (det_name,det_type,det_id);
  Volume      motherVol = lcdd.pickMotherVolume(sdet);
    
  for(xml_coll_t mi(x_det,_X(module)); mi; ++mi)  {
    xml_comp_t x_mod  = mi;
    xml_comp_t m_env  = x_mod.child(_X(module_envelope));
    string     m_nam  = det_name+"_"+x_mod.nameStr();
    Box        m_box (lcdd,m_nam+"_box",m_env.width(),m_env.length(),m_env.thickness());
    Volume     m_vol (lcdd,m_nam,m_box,air);
    DetElement m_elt (m_nam,det_type+"/Module",det_id);
    int        ncomponents = 0, sensor_number = 0;
    for(xml_coll_t ci(x_mod,_X(module_component)); ci; ++ci, ++ncomponents)  {
      xml_comp_t x_comp = ci;
      xml_comp_t x_pos  = x_comp.child(_X(position),false);
      xml_comp_t x_rot  = x_comp.child(_X(rotation),false);	
      string     c_nam  = m_nam + _toString(ncomponents,"_component%d");
      Box        c_box (lcdd,c_nam,x_comp.width(),x_comp.length(),x_comp.thickness());
      Volume     c_vol (lcdd,c_nam,c_box,lcdd.material(x_comp.materialStr()));
      DetElement c_elt (c_nam,det_type+"/Module/Component",det_id);
      PlacedVolume c_phv;
        
      if ( x_pos && x_rot ) {
	Position   c_pos(x_pos.x(0),x_pos.y(0),x_pos.z(0));
	Rotation   c_rot(x_rot.x(0),x_rot.y(0),x_rot.z(0));
	c_phv = m_vol.placeVolume(c_vol, c_pos, c_rot);
      }
      else if ( x_rot ) {
	c_phv = m_vol.placeVolume(c_vol,Rotation(x_rot.x(0),x_rot.y(0),x_rot.z(0)));
      }
      else if ( x_pos ) {
	c_phv = m_vol.placeVolume(c_vol,Position(x_pos.x(0),x_pos.y(0),x_pos.z(0)));
      }
      else {
	c_phv = m_vol.placeVolume(c_vol,IdentityPos());
      }
      if ( x_comp.isSensitive() ) {
	c_phv.addPhysVolID(_X(sensor),sensor_number++);
	c_vol.setSensitiveDetector(sens);
      }
      c_elt.setAttributes(lcdd,c_vol,x_comp.regionStr(),x_comp.limitsStr(),x_comp.visStr());
      m_elt.add(c_elt);
    }
    m_vol.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
    sdet.add(m_elt);
  }
  for(xml_coll_t li(x_det,_X(layer)); li; ++li)  {
    xml_comp_t x_layer  = li;
    xml_comp_t x_barrel = x_layer.child(_X(barrel_envelope));
    xml_comp_t x_layout = x_layer.child(_X(rphi_layout));
    xml_comp_t z_layout = x_layer.child(_X(z_layout));      // Get the <z_layout> element.
    int        lay_id   = x_layer.id();
    string     m_nam    = det_name+"_"+x_layer.moduleStr();
    DetElement m_elt    = sdet.child(m_nam);
    Volume     m_env    = lcdd.volume(m_nam);
    string     lay_nam  = det_name+_toString(lay_id,"_layer%d");
    Tube       lay_tub   (lcdd,lay_nam,x_barrel.inner_r(),x_barrel.outer_r(),x_barrel.z_length());
    Volume     lay_vol   (lcdd,lay_nam,lay_tub,air);  // Create the layer envelope volume.
    double     phi0     = x_layout.phi0();      // Starting phi of first module.
    double     phi_tilt = x_layout.phi_tilt();  // Phi tilt of a module.
    double     rc       = x_layout.rc();        // Radius of the module center.
    int        nphi     = x_layout.nphi();      // Number of modules in phi.
    double     rphi_dr  = x_layout.dr();        // The delta radius of every other module.
    double     phi_incr = (M_PI * 2) / nphi;    // Phi increment for one module.
    double     phic     = phi0;                 // Phi of the module center.
    double     z0       = z_layout.z0();        // Z position of first module in phi.
    double     nz       = z_layout.nz();        // Number of modules to place in z.
    double     z_dr     = z_layout.dr();        // Radial displacement parameter, of every other module.
      
    // Z increment for module placement along Z axis.
    // Adjust for z0 at center of module rather than
    // the end of cylindrical envelope.
    double z_incr   = (2.0 * z0) / (nz - 1);
    // Starting z for module placement along Z axis.
    double module_z = -z0;
    int module = 0;
      
    // Loop over the number of modules in phi.
    for (int ii = 0; ii < nphi; ii++)	{
      double dx = z_dr * std::cos(phic + phi_tilt);	// Delta x of module position.
      double dy = z_dr * std::sin(phic + phi_tilt);	// Delta y of module position.
      double  x = rc * std::cos(phic);                // Basic x module position.
      double  y = rc * std::sin(phic);               	// Basic y module position.
        
      // Loop over the number of modules in z.
      for (int j = 0; j < nz; j++)	  {
	// Create a unique name for the module in this logical volume, layer, phi, and z.
	string m_place = lay_nam + _toString(ii,"_phi%d") + _toString(j,"_z%d");
	// Module PhysicalVolume.
	PlacedVolume  m_physvol = 
          lay_vol.placeVolume(m_env,Position(x,y,module_z),
                              Rotation(M_PI/2,-((M_PI/2)-phic-phi_tilt),0));
	m_physvol.addPhysVolID("module", module++);
	// Adjust the x and y coordinates of the module.
	x += dx;
	y += dy;
	// Flip sign of x and y adjustments.
	dx *= -1;
	dy *= -1;
	// Add z increment to get next z placement pos.
	module_z += z_incr;
      }
      phic    += phi_incr;      // Increment the phi placement of module.
      rc      += rphi_dr;       // Increment the center radius according to dr parameter.
      rphi_dr *= -1;            // Flip sign of dr parameter.
      module_z = -z0;           // Reset the Z placement parameter for module.
    }
    m_elt.setAttributes(lcdd,lay_vol,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());
    // Create the PhysicalVolume for the layer.
    PlacedVolume lpv = motherVol.placeVolume(lay_vol); // Place layer in mother
    lpv.addPhysVolID("system", det_id);      // Set the subdetector system ID.
    lpv.addPhysVolID("barrel", 0);           // Flag this as a barrel subdetector.
    lpv.addPhysVolID("layer", lay_id);       // Set the layer ID.
  }
  return sdet;
}

DECLARE_DETELEMENT(SiTrackerBarrel,create_detector);
