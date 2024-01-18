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
// Specialized generic detector constructor for fiber tube calorimeter.
// See https://github.com/AIDASoft/DD4hep/issues/1173 for details
//
// Detector geometry structure
//                <system>  <layer>  <tube>  <hole> <type>
// /world_volume/FiberCalo/rowtube_1/brass_1/hole_1/quartz_1
//                                  /brass_2/hole_1/scintillator_1    brass_1 == brass_2 == brass....n
//                                  /brass_3/hole_1/quartz_1         
//                                  /brass_4/hole_1/scintillator_1
//                                  /brass_5/hole_1/quartz_1
// brass_1/quartz_1   Volume(brass) / hole
//                    Volume(hole)  / quartz
//              alt:  Volume(hole)  / scintillator
// 
// /world_volume/FiberCalo/rowtube_1/brass/quartz
//
// Dump using:
// $> geoPluginRun -input examples/ClientTests/compact/FiberTubeCalorimeter.xml 
//              -volmgr -print 3 -plugin DD4hep_DetectorCheck 
//              -name FiberTubeCalorimeter -geometry -structure -sensitive -volmgr -ignore
//
// Display using:
// $> geoDisplay examples/ClientTests/compact/FiberTubeCalorimeter.xml
//
//==========================================================================
#include <DD4hep/DetFactoryHelper.h>
#include <DD4hep/Printout.h>


#include <iomanip>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  constexpr double tol = 0.0;
  xml_det_t     x_det = e;

  // for volume tags in detector
  int           det_id    = x_det.id();
  string        det_name  = x_det.nameStr();
  Material      air       = description.air();

  // pointer to finding dimensins text in xml file
  // look in DDCore/include/Parsers/detail/Dimension.h for arguments
  xml_comp_t    x_dim     = x_det.dimensions();
  double        hthick    = x_dim.thickness();
  double        hzlength  = x_dim.z_length()/2.;
  
  // these refer to different fields in the xml file for this detector
  xml_comp_t fX_struct( x_det.child( _Unicode(structure) ) );
  xml_comp_t fX_absorb( fX_struct.child( _Unicode(absorb) ) );
  xml_comp_t fX_core1(  fX_struct.child( _Unicode(core1) ) );
  xml_comp_t fX_core2(  fX_struct.child( _Unicode(core2) ) );
  xml_comp_t fX_hole(   fX_struct.child( _Unicode(hole) ) );
  xml_comp_t fX_phdet1( fX_struct.child( _Unicode(phdet1) ) );
  xml_comp_t fX_phdet2( fX_struct.child( _Unicode(phdet2) ) );

  Material     mat;
  Transform3D  trafo;
  PlacedVolume pv;
  Solid        sol;

  sens.setType("calorimeter");
  //
  // scint fiber
  sol = Tube(0., fX_core1.rmax(), hzlength);
  mat = description.material(fX_core1.materialStr());
  Volume fiber_scint_vol(fX_core1.nameStr(), sol, mat);
  fiber_scint_vol.setAttributes(description, fX_core1.regionStr(), fX_core1.limitsStr(), fX_core1.visStr());
  if ( fX_core1.isSensitive() ) {
    fiber_scint_vol.setSensitiveDetector(sens);
  }
  cout << setw(28) << left << fiber_scint_vol.name()
       << " mat: "   << setw(15) << left << mat.name()
       << " vis: "   << setw(15) << left<< fX_core1.visStr()
       << " solid: " << setw(20) << left << sol.type()
       << " sensitive: " << yes_no(fX_core1.isSensitive()) << endl;
  //
  // quartz fiber
  sol = Tube(0., fX_core2.rmax(), hzlength);
  mat = description.material(fX_core2.materialStr());
  Volume  fiber_quartz_vol(fX_core2.nameStr(), sol, mat);
  fiber_quartz_vol.setAttributes(description, fX_core2.regionStr(), fX_core2.limitsStr(), fX_core2.visStr());
  if ( fX_core2.isSensitive() ) {
    fiber_quartz_vol.setSensitiveDetector(sens);
  }
  cout << setw(28) << left << fiber_quartz_vol.name()
       << " mat: " << setw(15) << left << mat.name()
       << " vis: " << setw(15) << left << fX_core2.visStr()
       << " solid: " << setw(20) << left << sol.type()
       << " sensitive: " << yes_no(fX_core2.isSensitive()) << endl;
  //
  // absorberhole with a scintillating fiber inside
  mat = description.material(fX_hole.materialStr());
  sol = Tube(0., fX_hole.rmax(), hzlength);
  Volume scint_hole_vol( fX_core1.nameStr()+"_hole", sol, mat);
  scint_hole_vol.setAttributes(description, fX_hole.regionStr(), fX_hole.limitsStr(), fX_hole.visStr());
  trafo = Transform3D(RotationZYX(0.,0.,0.),Position(0.,0.,0.));
  pv    = scint_hole_vol.placeVolume(fiber_scint_vol, trafo);
  pv.addPhysVolID("type",1);
  cout << setw(28) << left << scint_hole_vol.name()
       << " mat: " << setw(15) << left << mat.name()
       << " vis: " << setw(15) << left << fX_hole.visStr()
       << " solid: " << setw(20) << left << sol.type()
       << " sensitive: " << yes_no(fX_hole.isSensitive()) << endl;
  if ( fX_hole.isSensitive() ) {
    scint_hole_vol.setSensitiveDetector(sens);
  }
  //
  // absorberhole with a quartz inside
  Volume quartz_hole_vol( fX_core2.nameStr()+"_hole", sol, mat);
  quartz_hole_vol.setAttributes(description, fX_hole.regionStr(), fX_hole.limitsStr(), fX_hole.visStr());
  pv = quartz_hole_vol.placeVolume(fiber_quartz_vol);
  pv.addPhysVolID("type",2);
  cout << setw(28) << left << quartz_hole_vol.name()
       << " mat: " << setw(15) << left << mat.name()
       << " vis: " << setw(15) << left << fX_hole.visStr()
       << " solid: " << setw(20) << left << sol.type()
       << " sensitive: " << yes_no(fX_hole.isSensitive()) << endl;
  if ( fX_hole.isSensitive() ) {
    quartz_hole_vol.setSensitiveDetector(sens);
  }

  // absorber with scintillator inside
  mat = description.material(fX_absorb.materialStr());
  sol = Tube(0., hthick, hzlength);
  Volume scint_abs_vol( fX_core1.nameStr()+"_absorber", sol, mat);
  scint_abs_vol.setAttributes(description, fX_absorb.regionStr(), fX_absorb.limitsStr(), fX_absorb.visStr());
  pv = scint_abs_vol.placeVolume(scint_hole_vol);
  pv.addPhysVolID("hole",1);
  cout << setw(28) << left << scint_abs_vol.name()
       << " mat: " << setw(15) << left << mat.name()
       << " vis: " << setw(15) << left << fX_absorb.visStr()
       << " solid: " << setw(20) << left << sol.type()
       << " sensitive: " << yes_no(fX_absorb.isSensitive()) << endl;
  if ( fX_absorb.isSensitive() ) {
    scint_abs_vol.setSensitiveDetector(sens);
  }
  //
  // absorber with quartz inside
  mat = description.material(fX_absorb.materialStr());
  Volume quartz_abs_vol(fX_core2.nameStr()+"_absorber", sol, mat);
  quartz_abs_vol.setAttributes(description, fX_absorb.regionStr(), fX_absorb.limitsStr(), fX_absorb.visStr());
  pv = quartz_abs_vol.placeVolume(quartz_hole_vol, trafo);
  pv.addPhysVolID("hole",2);
  cout << setw(28) << left << quartz_abs_vol.name()
       << " mat: " << setw(15) << left << mat.name()
       << " vis: " << setw(15) << left << fX_absorb.visStr()
       << " solid: " << setw(20) << left << sol.type()
       << " sensitive: " << yes_no(fX_absorb.isSensitive()) << endl;
  if ( fX_absorb.isSensitive() ) {
    quartz_abs_vol.setSensitiveDetector(sens);
  }
  //
  // Setup the volumes with the shapes and properties in one horixontal layer
  int    Ncount = x_dim.numsides();
  double hzph   = x_dim.z1();
  double agap   = x_dim.gap();
  double dx     = 2*(Ncount + Ncount+1)/2e0 * (hthick+agap) + tol;
  double dy     = hthick + tol;
  double dz     = hzlength+hzph + tol;
  Box    tube_row_box(dx, dy, dz);
  Volume tube_row_vol("layer", tube_row_box, air);
  tube_row_vol.setVisAttributes(description, x_det.visStr());
  tube_row_vol.setSensitiveDetector(sens);
  cout << tube_row_vol.name()
       << " dx: " << tube_row_box.x()
       << " dy: " << tube_row_box.y()
       << " dz: " << tube_row_box.z() << endl;
  tube_row_vol.setVisAttributes(description, "layerVis");

  for (int ijk=-Ncount; ijk<Ncount+1; ijk++) {
    double mod_x_off = (ijk) * 2 * (hthick + agap);
    int towernum = Ncount + ijk + 1;
    pv = tube_row_vol.placeVolume((towernum%2 == 0) ? quartz_abs_vol : scint_abs_vol, Position(mod_x_off,0.,0.));
    pv.addPhysVolID("tube", towernum);
    cout << "Placing row  "    << setw(5) << right << ijk
         << " x-offset: "      << setw(7) << right << mod_x_off
         << " volume of type " << pv.volume().name()
         << endl;
  }

  dy = 2*(Ncount + Ncount+1)/2e0 * (hthick+agap) + tol;
  DetElement    sdet(det_name, det_id);
  Box           env_box(dx+tol, dy+tol, dz+tol);
  Volume        envelopeVol(det_name, env_box, air);
  envelopeVol.setAttributes(description, x_det.regionStr(), x_det.limitsStr(), x_det.visStr());
  if ( x_dim.isSensitive() )   {
    envelopeVol.setSensitiveDetector(sens);
  }
  //
  // Now stack multiple horizontal layers to form the final box
  for (int ijk=-Ncount; ijk < Ncount+1; ijk++) {
    double mod_y_off = (ijk) * 2 * (tube_row_box.y() + agap);
    Transform3D tr(RotationZYX(0.,0.,0.), Position(0.,mod_y_off,0.));
    pv = envelopeVol.placeVolume(tube_row_vol, tr);
    pv.addPhysVolID("layer", Ncount+ijk+1);

    DetElement de_layer(_toString(Ncount+ijk, "layer_%d"), det_id);
    de_layer.setPlacement(pv);
    sdet.add(de_layer);
    cout << "Placing " << setw(28) << left << de_layer.name()
         << " y-offset: "  << setw(7) << right << mod_y_off
         << " volume of type " << pv.volume().name()
         << endl;
  }
  //
  // detector element for entire detector.
  dz = x_dim.zmin()+hzlength+hzph+tol;
  Volume motherVol = description.pickMotherVolume(sdet);
  pv = motherVol.placeVolume(envelopeVol, Position(0, 0, dz));
  pv.addPhysVolID("system", det_id);
  sdet.setPlacement(pv);  // associate the placed volume to the detector element
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_FiberTubeCalorimeter,create_detector)
