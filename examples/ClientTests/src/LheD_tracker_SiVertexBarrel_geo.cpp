// $Id: LheSiTrackerBarrel_geo.cpp 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
  xml_det_t   x_det     = e;
  Material    air       = lcdd.air();
  int         det_id    = x_det.id();
  string      det_name  = x_det.nameStr();
  DetElement  sdet       (det_name,det_id);
  Assembly    assembly   (det_name+"_assembly");
  map<string, Volume>    volumes;
  PlacedVolume pv;

  double  mod_width;//       =   1;                         // (10.0 mm) module width
    
  sens.setType("tracker");
  for(xml_coll_t mi(x_det,_U(module)); mi; ++mi)  {
    xml_comp_t x_mod  = mi;
    xml_comp_t m_env  = x_mod.child(_U(module_envelope));
    string     m_nam  = x_mod.nameStr();
    mod_width = m_env.width();
    Volume     m_vol(det_name+"_"+m_nam,Box(m_env.width()/2,m_env.length()/2,m_env.thickness()/2),air);
    int        ncomponents = 0, sensor_number = 1;

    if ( volumes.find(m_nam) != volumes.end() )   {
      printout(ERROR,"SiTrackerBarrel","Logics error in building modules.");
      throw runtime_error("Logics error in building modules.");
    }
    volumes[m_nam] = m_vol;
        
    for(xml_coll_t ci(x_mod,_U(module_component)); ci; ++ci, ++ncomponents)  {
      xml_comp_t x_comp = ci;
      xml_comp_t x_pos  = x_comp.position(false);
      xml_comp_t x_rot  = x_comp.rotation(false);
      string     c_nam  = det_name+"_"+m_nam+_toString(ncomponents,"_component%d");
      Box        c_box(x_comp.width()/2,x_comp.length()/2,x_comp.thickness()/2);
      Volume     c_vol(c_nam,c_box,lcdd.material(x_comp.materialStr()));

      if ( x_pos && x_rot ) {
	Position    c_pos(x_pos.x(0),x_pos.y(0),x_pos.z(0));
	RotationZYX c_rot(x_rot.z(0),x_rot.y(0),x_rot.x(0));
	pv = m_vol.placeVolume(c_vol, Transform3D(c_rot,c_pos));
      }
      else if ( x_rot ) {
	pv = m_vol.placeVolume(c_vol,RotationZYX(x_rot.z(0),x_rot.y(0),x_rot.x(0)));
      }
      else if ( x_pos ) {
	pv = m_vol.placeVolume(c_vol,Position(x_pos.x(0),x_pos.y(0),x_pos.z(0)));
      }
      else {
	pv = m_vol.placeVolume(c_vol);
      }
      c_vol.setRegion(lcdd, x_comp.regionStr());
      c_vol.setLimitSet(lcdd, x_comp.limitsStr());
      c_vol.setVisAttributes(lcdd, x_comp.visStr());
      if ( x_comp.isSensitive() ) {
	pv.addPhysVolID(_U(sensor),sensor_number++);
	c_vol.setSensitiveDetector(sens);
      }
    }
    m_vol.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
  }
    
  for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
    xml_comp_t x_layer  = li;
    xml_comp_t x_barrel = x_layer.child(_U(barrel_envelope));
    xml_comp_t x_layout = x_layer.child(_U(rphi_layout));
    xml_comp_t z_layout = x_layer.child(_U(z_layout));      // Get the <z_layout> element.
    int        lay_id   = x_layer.id();
    string     m_nam    = x_layer.moduleStr();
    Volume     m_env    = volumes[m_nam];
    string     lay_nam  = det_name+"_"+m_nam+_toString(x_layer.id(),"_layer%d");

    // parameters from XML file
    //double     rrmax2    = x_layout.rmax2();          // add an attribute
    
    double     phi0     = x_layout.phi0();              // Starting phi of first module.
    double     phi_tilt = x_layout.phi_tilt();          // Phi tilt of a module.
    double     rc       = x_layout.rc();                // Radius of the module center.
    int        nphi     = x_layout.nphi();              // Number of modules in phi.
    double     rphi_dr  = x_layout.dr();                // The delta radius of every other module.
    //double     phi_incr = (2*M_PI) / nphi;            // Phi increment for one module.
    double     phic     = phi0;                         // Phi of the module center.
    double     z0       = z_layout.z0();                // Z position of first module in phi.
    double     nz       = z_layout.nz();                // Number of modules to place in z.
    double     z_dr     = z_layout.dr();                // Radial displacement parameter, of every other module.

    // multiplication factor for ellipse major radius
    // these values might be changed by rmax2
    double c0;                                      
    if (x_layer.id() <=1 ) c0 = 3.5;
    else if (x_layer.id() == 2 ) c0 = 2.3;
    else if (x_layer.id() == 3 ) c0 = 2;
    else c0 = 1.8;

    // create envelope        
    double env_rmin = x_barrel.inner_r();    // inner radius for envelope
    double env_rmax = x_barrel.outer_r();    // outer radius for envelope
    double env_z    = x_barrel.z_length();   // length of envelope

    EllipticalTube envElTubeOut(c0*env_rmax,env_rmax, env_z);
    EllipticalTube envElTubeInn(c0*env_rmin,env_rmin, env_z+0.01);
    SubtractionSolid envElTube(envElTubeOut,envElTubeInn);

    Tube envTube1(env_rmin, env_rmax, env_z+0.01, 3*M_PI/2, M_PI/2);
    UnionSolid lay_tub1(envElTube,envTube1);

    Tube envTube2(env_rmax, c0*env_rmax, env_z+0.01, 3*M_PI/2, M_PI/2);
    SubtractionSolid lay_tub(lay_tub1,envTube2);
    Volume     lay_vol   (lay_nam,lay_tub,air);         // Create the layer envelope volume.

    // ellipse circumference Ramanujan method
    double     a     = c0 * rc;    // (mm) ellipse major radius
    double     b     = rc;         // (mm) ellipse minor radius
    double cir_ellip = M_PI * ( 3*(a+b) - std::sqrt((3*a+b)* (a+3*b)));

    // circle circumference
    double cir_circl = 2 * M_PI * rc;

    int     nphi_ellip      = cir_ellip / mod_width / 2;   // number of modules for semi-ellipse in phi
    double  phi_incr_ellip  = (M_PI) / nphi_ellip;         // Phi increment for one module along semi-ellipse
    int     nphi_circl      = cir_circl / mod_width / 2;   // number of modules for semi-circle in phi
    double  phi_incr_circl  = (M_PI) / nphi_circl;         // Phi increment for one module along semi-circle

    std::cout << "nphi_ellip,phi_incr_ellip,nphi_circl,phi_incr_circl : "
	      << nphi_ellip << " " << phi_incr_ellip << " "
	      << nphi_circl << " " << phi_incr_circl << " " << mod_width << std::endl;

    // Z increment for module placement along Z axis.
    // Adjust for z0 at center of module rather than
    // the end of cylindrical envelope.
    double z_incr   = nz > 1 ? (2.0 * z0) / (nz - 1) : 0.0;
    // Starting z for module placement along Z axis.
    double module_z = -z0;
    int module = 1;

    double dx, dy, x, y;

    // Loop over the number of modules in phi.
    nphi = nphi_ellip + nphi_circl;
        
    for (int ii = 0; ii < nphi; ii++)    {
      dx = z_dr * std::cos(phic + phi_tilt);    // Delta x of module position.
      dy = z_dr * std::sin(phic + phi_tilt);    // Delta y of module position.

      // first half, semi-elliptical, from 90 to 270 degrees
      // second half, semi-circle, from 270 to 90 degrees    
      // Basic x module position.
      if (ii < nphi_ellip) x = c0 * rc * std::cos(phic); 
      else x = rc * std::cos(phic);
            
      y = rc * std::sin(phic);                  // Basic y module position.

      std::cout << x << " " << y << std::endl;
            
      // Loop over the number of modules in z.
      for (int j = 0; j < nz; j++)      {
	// Module PhysicalVolume.
	// Transform3D tr(RotationZYX(0,-((M_PI/2)-phic-phi_tilt),M_PI/2),Position(x,y,module_z));
	//NOTE (Nikiforos, 26/08 Rotations needed to be fixed so that component1 (silicon) is on the outside
	Transform3D tr(RotationZYX(0,((M_PI/2)-phic-phi_tilt),-M_PI/2),Position(x,y,module_z));
	pv = lay_vol.placeVolume(m_env,tr);
	pv.addPhysVolID("module", module++);
	// Adjust the x and y coordinates of the module.
	x += dx;
	y += dy;
	// Flip sign of x and y adjustments.
	dx *= -1;
	dy *= -1;
	// Add z increment to get next z placement pos.
	module_z += z_incr;
      }

      // first half, semi-elliptical, from 90 to 270 degrees
      // second half, semi-circle, from 270 to 90 degrees    
      // Increment the phi placement of module.
      if (ii < nphi_ellip) {
	// more increment in 150-210 degrees
	//if ( phic > 5*M_PI/6 && phic < 7*M_PI/6)  phic += 3*phi_incr_ellip;
	//else phic += phi_incr_ellip;                
	phic += phi_incr_ellip;                
      }
      else phic += phi_incr_circl;
            
      rc        += rphi_dr;       // Increment the center radius according to dr parameter.
      rphi_dr   *= -1;            // Flip sign of dr parameter.
      module_z   = -z0;           // Reset the Z placement parameter for module.
    }

    // Create the PhysicalVolume for the layer.
    assembly.setVisAttributes(lcdd.invisible());
    pv = assembly.placeVolume(lay_vol); // Place layer in mother
    pv.addPhysVolID("layer", lay_id);       // Set the layer ID.
    DetElement m_elt(sdet,lay_nam,lay_id);
    m_elt.setAttributes(lcdd,lay_vol,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());
    m_elt.setPlacement(pv);
  }
    
  pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system", det_id);      // Set the subdetector system ID.
  pv.addPhysVolID("barrel", 0);           // Flag this as a barrel subdetector.
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(LheD_tracker_SiVertexBarrel,create_detector)




/*
  21/2 + 58/2 = 39    // cylinderCircumference/2 + ellipseCircumference/2
  40/2 + 109/2 = 74   // cylinderCircumference/2 + ellipseCircumference/2
  91. * TMath::Pi() / 180.
  90 *deg  = 1.57079632679489656e+00 *rad
  270 *deg = 4.71238898038468967e+00 *rad
*/


