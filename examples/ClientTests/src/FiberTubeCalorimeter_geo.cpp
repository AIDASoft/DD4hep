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
// geoPluginRun -input examples/ClientTests/compact/FiberTubeCalorimeter.xml \
//              -volmgr -print 3 -plugin DD4hep_DetectorCheck \
//               -name FiberTubeCalorimeter -geometry -structure -sensitive -volmgr -ignore
//
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"

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
  double        hzph      = x_dim.z1();
  int           Ncount    = x_dim.numsides();
  double        agap      = x_dim.gap();
  double        azmin     = x_dim.zmin();

  // these refer to different fields in the xml file for this detector
  xml_comp_t fX_struct( x_det.child( _Unicode(structure) ) );
  xml_comp_t fX_absorb( fX_struct.child( _Unicode(absorb) ) );
  xml_comp_t fX_core1( fX_struct.child( _Unicode(core1) ) );
  xml_comp_t fX_core2( fX_struct.child( _Unicode(core2) ) );
  xml_comp_t fX_hole( fX_struct.child( _Unicode(hole) ) );
  xml_comp_t fX_phdet1( fX_struct.child( _Unicode(phdet1) ) );
  xml_comp_t fX_phdet2( fX_struct.child( _Unicode(phdet2) ) );
  
  // detector element for entire detector.  
  DetElement    sdet      (det_name, det_id);
  Volume        motherVol = description.pickMotherVolume(sdet);
  Box           env_box   ((2*Ncount+1)*(hthick+agap+tol),(2*Ncount+1)*(hthick+agap+tol), (hzlength+hzph+tol));
  Volume        envelopeVol  (det_name, env_box, air);
  envelopeVol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  Material mat;
  Transform3D trafo;
  PlacedVolume pv;
  
  pv = motherVol.placeVolume(envelopeVol, Position(0.,0.,azmin+hzlength+hzph+tol));
  pv.addPhysVolID("system", det_id);
  sdet.setPlacement(pv);  // associate the placed volume to the detector element
  sens.setType("calorimeter");

  // scint fiber
  mat = description.material(fX_core1.materialStr());
  Volume fiber_scint_vol("fiber1", Tube(0.,fX_core1.rmax(), hzlength), mat);
  fiber_scint_vol.setAttributes(description,fX_core1.regionStr(),fX_core1.limitsStr(),fX_core1.visStr());
  if ( fX_core1.isSensitive() ) {
    fiber_scint_vol.setSensitiveDetector(sens);
  }

  // quartz fiber
  Tube   fiber_quartz_tub(0.,fX_core2.rmax(), hzlength);
  Volume fiber_quartz_vol("fiber_quartz", fiber_quartz_tub, description.material(fX_core2.materialStr()));
  fiber_quartz_vol.setAttributes(description,fX_core2.regionStr(),fX_core2.limitsStr(),fX_core2.visStr());
  cout<<"fiber_quartz vis is "<<fX_core2.visStr()<<endl;
  if ( fX_core2.isSensitive() ) {
    cout<<"setting DRFtubeFiber fiber_quartz sensitive "<<endl;
    fiber_quartz_vol.setSensitiveDetector(sens);
  }

  // absorberhole with a scintillating fiber inside
  Tube   hole_tube(0.,fX_hole.rmax(),hzlength);
  Volume scint_hole_vol( "scintAbsorberHole", hole_tube, air);
  scint_hole_vol.setAttributes(description,fX_hole.regionStr(),fX_hole.limitsStr(),fX_hole.visStr());
  trafo = Transform3D(RotationZYX(0.,0.,0.),Position(0.,0.,0.));
  pv    = scint_hole_vol.placeVolume(fiber_scint_vol, trafo);
  pv.addPhysVolID("type",1);
  if ( fX_hole.isSensitive() ) {
    cout<<"setting DRFtubeFiber absorber hole sensitive "<<endl;
    scint_hole_vol.setSensitiveDetector(sens);
  }

  // absorberhole with a quartz inside
  Volume quartz_hole_vol( "quartzAbsorberHole", hole_tube, air);
  quartz_hole_vol.setAttributes(description,fX_hole.regionStr(),fX_hole.limitsStr(),fX_hole.visStr());
  pv = quartz_hole_vol.placeVolume(fiber_quartz_vol);
  pv.addPhysVolID("type",2);
  if ( fX_hole.isSensitive() ) {
    cout<<"setting DRFtubeFiber absorber hole sensitive "<<endl;
    quartz_hole_vol.setSensitiveDetector(sens);
  }

  // absorber with scintillator inside
  mat = description.material(fX_absorb.materialStr());
  Tube   brass_tube(0.,hthick,hzlength);
  Volume scint_abs_vol( "scintAbsorber", brass_tube, mat);
  scint_abs_vol.setAttributes(description,fX_absorb.regionStr(),fX_absorb.limitsStr(),fX_absorb.visStr());
  pv = scint_abs_vol.placeVolume(scint_hole_vol);
  pv.addPhysVolID("hole",1);
  if ( fX_absorb.isSensitive() ) {
    cout<<"setting DRFtubeFiber absorber sensitive "<<endl;
    scint_abs_vol.setSensitiveDetector(sens);
  }

  // absorber with quartz inside
  mat = description.material(fX_absorb.materialStr());
  Volume quartz_abs_vol( "quartzAbsorber", brass_tube, mat);
  quartz_abs_vol.setAttributes(description,fX_absorb.regionStr(),fX_absorb.limitsStr(),fX_absorb.visStr());
  pv = quartz_abs_vol.placeVolume(quartz_hole_vol, trafo);
  pv.addPhysVolID("hole",2);
  if ( fX_absorb.isSensitive() ) {
    cout<<"setting DRFtubeFiber absorber sensitive "<<endl;
    quartz_abs_vol.setSensitiveDetector(sens);
  }

  // setup the volumes with the shapes and properties in one horixontal layer
  Volume tube_row_vol("layer", Box(hthick,hthick,hzlength+hzph), air);
  tube_row_vol.setVisAttributes(description, x_det.visStr());
  //tube_row_vol.setSensitiveDetector(sens);
  
  for (int ijk=-Ncount; ijk<Ncount+1; ijk++) {
    cout<<"making row ijk "<<ijk<<endl;
    double mod_x_off = (ijk)*2*(hthick+agap);
    Transform3D tr(RotationZYX(0.,0.,0.), Position(mod_x_off,0.,0.));
    int towernum = Ncount + ijk + 1;
    pv = tube_row_vol.placeVolume((towernum%2 == 0) ? quartz_abs_vol : scint_abs_vol, tr);
    pv.addPhysVolID("tube", towernum);
  }
  // Now stack multiple horizontal layers to form the final box
  for (int ijk=-Ncount; ijk<Ncount+1; ijk++) {
    cout << "making row ijk " << ijk << endl;
    double mod_y_off = (ijk)*2*(hthick+agap);
    Transform3D tr(RotationZYX(0.,0.,0.), Position(0.,mod_y_off,0.));
    pv = envelopeVol.placeVolume(tube_row_vol, tr);
    pv.addPhysVolID("layer", Ncount+ijk+1);

    DetElement de_layer(_toString(Ncount+ijk, "layer_%d"), det_id);
    de_layer.setPlacement(pv);
    sdet.add(de_layer);
  }
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_FiberTubeCalorimeter,create_detector)
