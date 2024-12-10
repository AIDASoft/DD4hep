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
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/Printout.h>


#include <iomanip>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  constexpr double tol = 1e-4;
  xml_det_t     x_det = e;
  
  // for volume tags in detector
  int           det_id     = x_det.id();
  string        det_name   = x_det.nameStr();
  DetElement    sdet(det_name, det_id);

  // pointer to finding dimensins text in xml file
  // look in DDCore/include/Parsers/detail/Dimension.h for arguments
  xml_comp_t    x_dim      = x_det.dimensions();
  double        cyl_rmin   = x_dim.rmin();
  double        cyl_rmax   = x_dim.rmax();
  double        cyl_dz     = x_dim.dz();
  double        layer_cnt  = x_dim.layers();

  xml_comp_t    x_wire     = x_det.child(_Unicode(wire));
  size_t        wire_cnt   = x_wire.count();
  double        wire_thick = x_wire.thickness();

  double        layer_thickness = (cyl_rmax - cyl_rmin)/double(layer_cnt+1);
  double        delta_phi = 2.0 * M_PI / double(wire_cnt);
  double        phi_start = 0e0;

  Solid         sdet_cyl = Tube(cyl_rmin-tol,cyl_rmax+tol,cyl_dz+tol);
  Volume        sdet_vol(det_name+"_vol", sdet_cyl, description.air());

  Material      wire_mat(description.material(x_wire.materialStr()));
  Tube          wire_cyl(0e0, wire_thick, cyl_dz-1.0*dd4hep::cm);
  Volume        wire_vol("Wire_vol", wire_cyl, wire_mat);
  PlacedVolume  pv;

  /// The Geant4 voxelization change must be applied to the parent volume
  if ( x_dim.hasAttr(_U(option)) )  {
    int value = x_dim.attr<int>(_U(option));
    printout(ALWAYS, "DriftChamber", "+++ Setting smartlessValue to %d for %s",
             value, sdet_vol.name());
    sdet_vol.setSmartlessValue(value);
  }
  /// Set volume attributes
  sdet_vol.setAttributes(description, x_det.regionStr(), x_det.limitsStr(), x_det.visStr());
  wire_vol.setVisAttributes(description.visAttributes(x_wire.visStr()));
  /// Place the wires in layers around the origin
  for( std::size_t l=0; l<layer_cnt; ++l )  {
    double radius = cyl_rmin + (0.5+double(l)) * layer_thickness;
    double phi_s  = phi_start + delta_phi * 0.5 * ((l%2) - 1.0);
    for( std::size_t i=0; i<wire_cnt; ++i )  {
      double rho = phi_s + delta_phi * i;
      double x   = std::cos(rho) * radius;
      double y   = std::sin(rho) * radius;
      // Volume wire_vol("Wire_"+std::to_string(l)+"_"+std::to_string(i), wire, wire_mat);
      wire_vol.setSensitiveDetector(sens);
      pv = sdet_vol.placeVolume(wire_vol, Position(x, y, 0e0));
      pv.addPhysVolID("layer", l);
      pv.addPhysVolID("phi", i);    
    }
  }

  Volume motherVol = description.pickMotherVolume(sdet);
  pv = motherVol.placeVolume(sdet_vol, Position(0e0, 0e0, 0e0));
  pv.addPhysVolID("system", det_id);
  sdet.setPlacement(pv);  // associate the placed volume to the detector element
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_DriftChamber,create_detector)
