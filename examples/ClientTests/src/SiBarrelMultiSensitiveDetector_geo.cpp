//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  typedef vector<PlacedVolume> Placements;
  xml_det_t   x_det     = e;
  Material    air       = description.air();
  int         det_id    = x_det.id();
  string      det_name  = x_det.nameStr();
  DetElement  sdet       (det_name,det_id);
  Assembly    assembly   (det_name);
  map<string, Volume>    volumes;
  map<string, Placements>  sensitives;
  PlacedVolume pv;

  sens.setType("tracker");
  for(xml_coll_t mi(x_det,_U(module)); mi; ++mi)  {
    xml_comp_t x_mod  = mi;
    xml_comp_t m_env  = x_mod.child(_U(module_envelope));
    string     m_nam  = x_mod.nameStr();
    Volume     m_vol(m_nam,Box(m_env.width()/2,m_env.length()/2,m_env.thickness()/2),air);
    int        ncomponents = 0, sensor_number = 1;

    if ( volumes.find(m_nam) != volumes.end() )   {
      printout(ERROR,"SiTrackerBarrel","Logics error in building modules.");
      throw runtime_error("Logics error in building modules.");
    }
    volumes[m_nam] = m_vol;
    m_vol.setVisAttributes(description.visAttributes(x_mod.visStr()));
    printout(INFO,"Detector","++ Building module: %s",m_nam.c_str());
    for(xml_coll_t ci(x_mod,_U(module_component)); ci; ++ci, ++ncomponents)  {
      xml_comp_t x_comp = ci;
      xml_comp_t x_pos  = x_comp.position(false);
      xml_comp_t x_rot  = x_comp.rotation(false);        
      string     c_nam  = _toString(ncomponents,"component%d");
      Box        c_box(x_comp.width()/2,x_comp.length()/2,x_comp.thickness()/2);
      Volume     c_vol(c_nam,c_box,description.material(x_comp.materialStr()));

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
      c_vol.setRegion(description, x_comp.regionStr());
      c_vol.setLimitSet(description, x_comp.limitsStr());
      c_vol.setVisAttributes(description, x_comp.visStr());
      if ( x_comp.isSensitive() ) {
        pv.addPhysVolID(_U(sensor),sensor_number++);
        c_vol.setSensitiveDetector(sens);
        sensitives[m_nam].push_back(pv);
      }
    }
  }
  for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
    xml_comp_t x_layer  = li;
    xml_comp_t x_barrel = x_layer.child(_U(barrel_envelope));
    xml_comp_t x_layout = x_layer.child(_U(rphi_layout));
    xml_comp_t z_layout = x_layer.child(_U(z_layout));      // Get the <z_layout> element.
    int        lay_id   = x_layer.id();
    string     m_nam    = x_layer.moduleStr();
    string     lay_nam  = _toString(x_layer.id(),"layer%d");
    Tube       lay_tub   (x_barrel.inner_r(),x_barrel.outer_r(),x_barrel.z_length()/2);
    Volume     lay_vol   (lay_nam,lay_tub,air);         // Create the layer envelope volume.
    double     phi0     = x_layout.phi0();              // Starting phi of first module.
    double     phi_tilt = x_layout.phi_tilt();          // Phi tilt of a module.
    double     rc       = x_layout.rc();                // Radius of the module center.
    int        nphi     = x_layout.nphi();              // Number of modules in phi.
    double     rphi_dr  = x_layout.dr();                // The delta radius of every other module.
    double     phi_incr = (M_PI * 2) / nphi;            // Phi increment for one module.
    double     phic     = phi0;                         // Phi of the module center.
    double     z0       = z_layout.z0();                // Z position of first module in phi.
    double     nz       = z_layout.nz();                // Number of modules to place in z.
    double     z_dr     = z_layout.dr();                // Radial displacement parameter, of every other module.
    Volume     m_env    = volumes[m_nam];
    DetElement lay_elt(sdet,_toString(x_layer.id(),"layer%d"),lay_id);
    Placements& sensVols = sensitives[m_nam];

    // Z increment for module placement along Z axis.
    // Adjust for z0 at center of module rather than
    // the end of cylindrical envelope.
    double z_incr   = nz > 1 ? (2.0 * z0) / (nz - 1) : 0.0;
    // Starting z for module placement along Z axis.
    double module_z = -z0;
    int module = 1;

    printout(INFO,"Detector","++ Placing module: %s",m_nam.c_str());
    // Loop over the number of modules in phi.
    for (int ii = 0; ii < nphi; ii++)        {
      double dx = z_dr * std::cos(phic + phi_tilt);        // Delta x of module position.
      double dy = z_dr * std::sin(phic + phi_tilt);        // Delta y of module position.
      double  x = rc * std::cos(phic);                     // Basic x module position.
      double  y = rc * std::sin(phic);                     // Basic y module position.

      // Loop over the number of modules in z.
      for (int j = 0; j < nz; j++)          {
        // Module PhysicalVolume.
        //         Transform3D tr(RotationZYX(0,-((M_PI/2)-phic-phi_tilt),M_PI/2),Position(x,y,module_z));
        //NOTE (Nikiforos, 26/08 Rotations needed to be fixed so that component1 (silicon) is on the outside
        Transform3D tr(RotationZYX(0,((M_PI/2)-phic-phi_tilt),-M_PI/2),Position(x,y,module_z));
        pv = lay_vol.placeVolume(m_env,tr);
        pv.addPhysVolID("module", module);

        if ( 0 == (module%2) )  {
          // Not terribly physical: we only make a child-element for every second module.
          // Need this for testing the volume manager!
          string module_name = _toString(module,"module%d");
          DetElement mod_elt(lay_elt,module_name,module);
          mod_elt.setPlacement(pv);
#if 0
          ::printf("+++ Module: %s  %s [%p]",
                 module_name.c_str(),pv->GetMatrix()->GetName(),(void*)pv->GetMatrix());
          pv->GetMatrix()->Print();
#endif
          for(size_t ic=0; ic<sensVols.size(); ++ic)  {
            PlacedVolume sens_pv = sensVols[ic];
            DetElement comp_elt(mod_elt,sens_pv.volume().name(),module);
            comp_elt.setPlacement(sens_pv);
          }
        }

        /// Increase counters etc.
        module++;
        // Adjust the x and y coordinates of the module.
        x += dx;
        y += dy;
        // Flip sign of x and y adjustments.
        dx *= -1;
        dy *= -1;
        // Add z increment to get next z placement pos.
        module_z += z_incr;
      }
      phic     += phi_incr;      // Increment the phi placement of module.
      rc       += rphi_dr;       // Increment the center radius according to dr parameter.
      rphi_dr  *= -1;            // Flip sign of dr parameter.
      module_z  = -z0;           // Reset the Z placement parameter for module.
    }
    // Create the PhysicalVolume for the layer.
    pv = assembly.placeVolume(lay_vol); // Place layer in mother
    pv.addPhysVolID("layer", lay_id);       // Set the layer ID.
    lay_elt.setAttributes(description,lay_vol,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());
    lay_elt.setPlacement(pv);
  }
  sdet.setAttributes(description,assembly,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
  assembly.setVisAttributes(description.invisible());
  pv = description.pickMotherVolume(sdet).placeVolume(assembly);
  pv.addPhysVolID("system", det_id);      // Set the subdetector system ID.
  pv.addPhysVolID("barrel", 0);           // Flag this as a barrel subdetector.
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_SiBarrelMultiSensitiveDetector,create_detector)
