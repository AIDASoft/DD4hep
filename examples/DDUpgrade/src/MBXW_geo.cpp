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
#include "XML/Utilities.h"

using namespace std;
using namespace dd4hep;

namespace {
  struct Builder : public xml::tools::VolumeBuilder  {
    double   coil_x;
    double   coil_y;
    double   coil_z;
    double   yoke_x;
    double   yoke_y;
    double   yoke_z;
    double   coil_hole_x;
    double   coil_hole_y;
    double   coil_hole_pos_x;
    std::map<string,double> params;

    Builder(Detector& description, xml_h e);
    double param(const std::string& name)   const;
    DetElement makeMagnet();
    Volume makeYoke(const char* name);
    Volume makeSpacer(const char* name);
    Volume makeCoil(const char* name, double curve_rad, double curve_thick);
  };
}

Builder::Builder(Detector& d, xml_h e) : VolumeBuilder(d,e)   {
  for(xml_coll_t c(e,_U(parameter)); c; ++c)  {
    xml_dim_t v = c;
    params.insert(make_pair(v.nameStr(), _toDouble(v.valueStr())));
  }
  coil_x = param("MBXW:CoilX");
  coil_y = param("MBXW:CoilY");
  coil_z = param("MBXW:CoilZ");
  yoke_x = param("MBXW:YokeX");
  yoke_y = param("MBXW:YokeY");
  yoke_z = coil_z*2.0;
  coil_hole_x = coil_x + 1.0*mm;
  coil_hole_y = param("MBXW:CoilHoleY");
  coil_hole_pos_x = param("MBXW:CoilHolePosX");
}

double Builder::param(const std::string& name)  const  {
  auto i = params.find(name);
  if ( i != params.end() ) return (*i).second;
  return _toDouble(name);
}

Volume Builder::makeSpacer(const char* name)    {
  Material mat      = materials["spacer"];
  double   spacer_x = coil_x/2.0;
  double   spacer_y = param("MBXW:SpacerY")/2.0;
  double   spacer_z = yoke_z/2.0;
  Box      solid(spacer_x, spacer_y, spacer_z);
  Volume   vol(name,solid,mat);
  vol.setVisAttributes(description,"MBXW:SpacerVis");
  return vol;
}

Volume Builder::makeYoke(const char* name)    {
  Material mat         = materials["yoke"];
  double   hole_x      = param("MBXW:CenterHoleX")/2.0;
  double   hole_y      = param("MBXW:CenterHoleY")/2.0;
  double   hole_z      = (yoke_z + 1.0*mm)/2.0;
  double   coil_hole_z = hole_z;

  Box      yoke(yoke_x/2.0, yoke_y/2.0, yoke_z/2.0);
  Box      hole(hole_x, hole_y, hole_z);
  Box      coil_hole(coil_hole_x/2.0, coil_hole_y/2.0, coil_hole_z);
  Solid    solid = SubtractionSolid(yoke, hole);
  solid = SubtractionSolid(solid, coil_hole, Position(-coil_hole_pos_x,0,0));
  solid = SubtractionSolid(solid, coil_hole, Position( coil_hole_pos_x,0,0));
  Volume vol(name,solid,mat);
  vol.setVisAttributes(description,"MBXW:YokeVis");
  return vol;
}

Volume Builder::makeCoil(const char* name, double curve_rad, double curve_thick)  {
  Material mat       = materials["coil"];
  double curve_dz    = coil_y/2.0;
  double curve_rmin  = curve_rad;
  double curve_rmax  = curve_rad + curve_thick;
  double curve_pos_x = curve_rad + curve_thick/2.0;
  double curve_pos_z = coil_z/2.0 + 0.01*mm;
  double line_x      = coil_hole_pos_x + coil_x/2.0 - curve_thick - curve_rad - 0.01*mm;
  double line_y      = coil_y;
  double line_z      = coil_x;
  double line_pos_x  = line_x/2.0 - coil_x/2.0 + curve_thick + curve_rad;
  double line_pos_z  = coil_z/2.0 - coil_x/2.0 + curve_thick + curve_rad;    

  Box         straight(coil_x/2.0, coil_y/2.0, coil_z/2.0);
  Tube        curve   (curve_rmin, curve_rmax, curve_dz, M_PI/2.0);
  Box         line    (line_x/2.0, line_y/2.0, line_z/2.0);
  Position    pos     (-curve_pos_x, 0, curve_pos_z);
  RotationZYX rot     (0,0,M_PI/2.0);
  Transform3D trafo = Transform3D(pos) * Transform3D(rot);
  Solid       solid = UnionSolid(straight, curve, trafo);
  solid = UnionSolid(solid, line, Position(-line_pos_x,0,line_pos_z));
  Volume vol(name,solid,mat);
  vol.setVisAttributes(description,"MBXW:CoilVis");
  return vol;
}

DetElement Builder::makeMagnet()   {
  Assembly    mbxw_vol(string("lv")+detector.name());
  Transform3D rotX (RotationZYX(0, 0, M_PI));
  Transform3D rotZ (RotationZYX(M_PI, 0, 0));
  Transform3D rotXZ(RotationZYX(M_PI, 0, M_PI));

  collectMaterials(x_det.child(_U(materials)));

  double coil_pos_y = (coil_hole_y - coil_y - 1.0*mm)/2.0;
  double coil_pos_z = coil_z/2.0 + 0.01*mm;
  Volume yoke       = makeYoke("lvMagnetYoke");
  Volume coil_up    = makeCoil("lvMagnetCoilUp",  param("MBXW:UpCurveRad"), coil_x);
  Volume coil_dwn   = makeCoil("lvMagnetCoilDown",param("MBXW:DownCurveRad"), coil_x);
  Volume spacer     = makeSpacer("lvMagnetBetweenCoil");

  mbxw_vol.placeVolume(yoke);
  mbxw_vol.placeVolume(spacer,               Position(-coil_hole_pos_x, 0, 0));
  mbxw_vol.placeVolume(spacer,   Transform3D(Position(coil_hole_pos_x, 0, 0))*rotZ);

  mbxw_vol.placeVolume(coil_dwn,             Position( coil_hole_pos_x, coil_pos_y, coil_pos_z));
  mbxw_vol.placeVolume(coil_up,  Transform3D(Position( coil_hole_pos_x, coil_pos_y,-coil_pos_z))*rotX);
  mbxw_vol.placeVolume(coil_dwn, Transform3D(Position(-coil_hole_pos_x, coil_pos_y, coil_pos_z))*rotZ);
  mbxw_vol.placeVolume(coil_up,  Transform3D(Position(-coil_hole_pos_x, coil_pos_y,-coil_pos_z))*rotXZ);

  mbxw_vol.placeVolume(coil_dwn,             Position( coil_hole_pos_x,-coil_pos_y, coil_pos_z));
  mbxw_vol.placeVolume(coil_up,  Transform3D(Position( coil_hole_pos_x,-coil_pos_y,-coil_pos_z))*rotX);
  mbxw_vol.placeVolume(coil_dwn, Transform3D(Position(-coil_hole_pos_x,-coil_pos_y, coil_pos_z))*rotZ);
  mbxw_vol.placeVolume(coil_up,  Transform3D(Position(-coil_hole_pos_x,-coil_pos_y,-coil_pos_z))*rotXZ);
  placeDetector(mbxw_vol);
  return detector;
}

static Ref_t create_element(Detector& description, xml_h e, Ref_t /* sens_det */)  {
  Builder b(description, e);
  return b.makeMagnet();
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(LHCb_MBXW,create_element)
