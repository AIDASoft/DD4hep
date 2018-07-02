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
#include "DD4hep/Printout.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;


static Ref_t create_element(Detector& description, xml_h e, Ref_t sens)  {
  xml_det_t  x_det  (e);
  xml_comp_t x_envelope = x_det.envelope();
  xml_dim_t  pos        = x_det.position();
  xml_dim_t  rot        = x_det.rotation();
  string     det_name   = x_det.nameStr();
  DetElement sdet(det_name,x_det.id());
  double     env_angle  = x_envelope.angle();
  double     env_dz     = x_envelope.dz()/2.0;
  double     sin_cone   = std::sin(std::atan(x_envelope.rmax()/env_dz/2.0));
  double     cos_beam   = std::cos(env_angle);
  double     sin_beam   = std::sin(env_angle);
  //Cone       env_cone(env_dz,0,0.0001,x_envelope.rmax(),x_envelope.rmax()+0.0001);
  //Volume     env_vol (det_name,env_cone,description.air());
  // If the cone should be removed and replaced by an assembly,
  // comment the upper to and enable the lower line
  Assembly   env_vol(det_name);
  PlacedVolume pv;
  SensitiveDetector sd = sens;

  sd.setType("tracker");

#if 0
  Box b(2000,2000,2000);
  Volume vb(det_name+"_bbb",b,description.material("Si"));
  pv = env_vol.placeVolume(vb, Position(0,0,0));
  vb.setSensitiveDetector(sd);
  pv.addPhysVolID("type", 0);
  pv.addPhysVolID("station", 20);
#endif
  
  // Set visualization, limits and region (if present)
  env_vol.setRegion(description, x_det.regionStr());
  env_vol.setLimitSet(description, x_det.limitsStr());
  env_vol.setVisAttributes(description, x_det.visStr());
  printout(INFO,"CODEX-b","%s beam-angle=%g atan(cone)=%g",
           det_name.c_str(), env_angle, std::atan(x_envelope.rmax()/env_dz/2.0));

  Tube tub(0, 3, x_envelope.dz()+500);
  Volume tub_vol(det_name+"_Tube", tub, description.air());
  tub_vol.setVisAttributes(description, "BlackVis");
  pv = env_vol.placeVolume(tub_vol, Position(0,0,0));

  int num_sensitive = 0;
  for(xml_coll_t i(x_det,_U(shield)); i; ++i)  {
    xml_comp_t shield = i;
    double     z  = shield.z(), dz = shield.dz();
    double     r1 = sin_cone*z, r2 = sin_cone*(z+dz);
    string     nam = det_name+"_"+shield.nameStr();
    Cone       con(dz/2., 0., r1, 0., r2);
    Material   mat(description.material(shield.attr<string>(_U(material))));
    Volume     vol(nam,con,mat);

    printout(INFO,"CODEX-b","%s Shield: %-12s %-12s z=%7g dz=%7g r1=%7g r2=%7g",
             det_name.c_str(), vol.name(), ('['+string(mat.name())+']').c_str(), z, dz, r1, r2);
    vol.setVisAttributes(description, shield.visStr());
    pv = env_vol.placeVolume(vol, Position(0,0,-env_dz+z+dz/2.0));
    if ( shield.isSensitive() )   {
      DetElement det(sdet, "shield_"+nam, x_det.id());
      vol.setSensitiveDetector(sd);
      pv.addPhysVolID("type", 0);
      pv.addPhysVolID("station", num_sensitive);
      det.setPlacement(pv);
      ++num_sensitive;
    }
  }

  int type_num = 0, station_number = 0;
  double z0 = 0.0, x0 = 0.0, x_tot = 0.0;
  for(xml_coll_t i(x_det,_U(station)); i; ++i, ++type_num)  {
    xml_comp_t station = i;
    x_tot += double(station.repeat())*(station.distance()+station.thickness());
  }
  printout(INFO,"CODEX-b","%s X_tot= %g",det_name.c_str(), x_tot);
  for(xml_coll_t i(x_det,_U(station)); i; ++i, ++type_num)  {
    xml_comp_t station = i;
    int        repeat = station.repeat();
    string     nam = _toString(station_number,"CODEX_station_type%d");
    Box        box(station.width()/2., station.height()/2., station.thickness()/2.);
    Material   mat(description.material(station.attr<string>(_U(material))));
    Volume     vol(nam,box,mat);
    double     dist = station.thickness()+station.distance();
    double     dx = dist/sin_beam;
    double     dz = dist/cos_beam;
    if ( station.hasAttr(_U(z)) ) {
      z0 = station.z()+x_tot/2.0/cos_beam;
      x0 = -x_tot/2.0/sin_beam;
    }
    // Set visualization, limits and region (if present)
    vol.setVisAttributes(description, station.visStr());
    vol.setLimitSet(description,  station.limitsStr());
    vol.setRegion(description, station.regionStr());
    if ( station.isSensitive() )   {
      vol.setSensitiveDetector(sd);
    }
    for(int j=0; j < repeat; ++j)    {
      DetElement det(sdet, _toString(station_number,"module%d"), station_number);
      pv = env_vol.placeVolume(vol, Transform3D(RotationZYX(0,M_PI/2.0-env_angle,0), Position(x0,0,z0)));
      pv.addPhysVolID("type", 1);
      pv.addPhysVolID("station", station_number);
      printout(INFO,"CODEX-b","%s Module: %2d %-12s x=%g y=%g z=%7g Dist:%g dx:%g dz:%g",
               det_name.c_str(), station_number, ('['+string(mat.name())+']').c_str(),
               x0, 0.0, z0, dist, dx, dz);
      det.setPlacement(pv);
      x0 += dx;
      z0 += dz;
      ++station_number;
    }
  }

  Volume mother = description.pickMotherVolume(sdet);
  Transform3D trafo(RotationZYX(rot.z(),rot.y(),rot.x()),Position(-pos.x(),-pos.y(),pos.z()));
  Direction dir = trafo*Direction(0,0,1.);
  pv = mother.placeVolume(env_vol,trafo);
  if ( x_det.hasAttr(_U(id)) )  {
    pv.addPhysVolID("system",x_det.id());
  }
  printout(INFO,det_name,"+++ Need to shot in direction: (x,y,z) = (%g,%g,%g)",dir.X(),dir.Y(),dir.Z());
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_CODEXb,create_element)

