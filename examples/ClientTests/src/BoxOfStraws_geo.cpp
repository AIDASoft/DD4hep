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
//
// Display using:
// $> geoDisplay examples/ClientTests/compact/BoxOfStraws.xml
//
//==========================================================================
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/DD4hepUnits.h>
#include <DD4hep/Printout.h>

using namespace dd4hep;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  double       tol     = 1e-5 * dd4hep::mm;
  xml_det_t    x_det   = e;
  xml_dim_t    x_box   = x_det.child(_U(box));
  xml_dim_t    x_rot   = x_det.child(_U(rotation));
  xml_dim_t    x_pos   = x_det.child(_U(position));
  xml_det_t    x_straw = x_det.child(_Unicode(straw));
  xml_det_t    x_gas   = x_det.child(_Unicode(gas));
  std::string  nam     = x_det.nameStr();
  const double thick   = x_straw.thickness();
  const double delta   = 2e0*x_straw.rmax();
  const int    num_x   = int(2e0*x_box.x() / delta);
  const int    num_z   = int(2e0*x_box.z() / (delta+2*tol));

  Tube   straw(0., x_straw.rmax()-tol, x_straw.y()-tol);
  Volume straw_vol("straw", straw, description.material(x_straw.materialStr()));
  straw_vol.setAttributes(description, x_straw.regionStr(), x_straw.limitsStr(), x_straw.visStr());
  
  Tube   straw_gas(0., straw.rMax()-thick, straw.dZ()-thick);
  Volume straw_gas_vol("gas", straw_gas, description.material(x_gas.materialStr()));
  straw_gas_vol.setAttributes(description, x_gas.regionStr(), x_gas.limitsStr(), x_gas.visStr());

  straw_vol.placeVolume(straw_gas_vol);

  printout(INFO, "BoxOfStraws", "%s: Straw: rmax: %7.3f y: %7.3f mat: %s vis: %s solid: %s",
           nam.c_str(), x_straw.rmax(), x_straw.y(), x_straw.materialStr().c_str(),
           x_straw.visStr().c_str(), straw.type());
  if( x_gas.hasChild(_U(sensitive)) )  {
    sens.setType("tracker");
    straw_gas_vol.setSensitiveDetector(sens);
  }

  // Envelope: make envelope box 'tol' bigger on each side, so that the straws
  Box    box(x_box.x()+tol, x_box.y()+tol, x_box.z()+tol);
  Volume box_vol(nam, box, description.air());
  box_vol.setAttributes(description, x_box.regionStr(), x_box.limitsStr(), x_box.visStr());

  Box    layer(x_box.x(), x_box.y(), x_straw.rmax());
  Volume layer_vol("layer", layer, description.air());
  layer_vol.setVisAttributes(description.visAttributes("VisibleGray"));
  
  printout(INFO, "BoxOfStraws", "%s: Layer:   nx: %7d nz: %7d delta: %7.3f", nam.c_str(), num_x, num_z, delta);
  Rotation3D rot(RotationZYX(0e0, 0e0, M_PI/2e0));
  for( int ix=0; ix < num_x; ++ix )  {
    double x = -box.x() + (double(ix)+0.5) * (delta + 2e0*tol);
    PlacedVolume pv = layer_vol.placeVolume(straw_vol, Transform3D(rot,Position(x, 0e0, 0e0)));
    pv.addPhysVolID("straw", ix+1);
  }
  for( int iz=0; iz < num_z; ++iz )  {
    // leave 'tol' space between the layers
    double z = -box.z() + (double(iz)+0.5) * (2.0*tol + delta);
    PlacedVolume pv = box_vol.placeVolume(layer_vol, Position(0e0, 0e0, z));
    pv.addPhysVolID("layer", iz+1);
  }
  printout(INFO, "BoxOfStraws", "%s: Created %d layers of %d straws each.", nam.c_str(), num_z, num_x);
  
  DetElement   sdet  (nam, x_det.id());
  Volume       mother(description.pickMotherVolume(sdet));
  Rotation3D   rot3D (RotationZYX(x_rot.z(0), x_rot.y(0), x_rot.x(0)));
  Transform3D  trafo (rot3D, Position(x_pos.x(0), x_pos.y(0), x_pos.z(0)));
  PlacedVolume pv = mother.placeVolume(box_vol, trafo);
  pv.addPhysVolID("system", x_det.id());
  sdet.setPlacement(pv);  // associate the placed volume to the detector element
  printout(INFO, "BoxOfStraws", "%s: Detector construction finished.", nam.c_str());
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_BoxOfStraws,create_detector)
