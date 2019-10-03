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
#include "DD4hep/Printout.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens)  {
  xml_det_t  x_det  (e);
  SensitiveDetector sd = sens;
  xml_dim_t  x_pos   = x_det.child(_U(position),false);
  xml_dim_t  x_rot   = x_det.child(_U(rotation),false);
  xml_dim_t  x_refl  = x_det.child(_U(reflect),false);
  string     ref_nam = x_det.attr<string>(_U(sdref));
  DetElement ref_det = description.detector(ref_nam);
  auto       refl    = ref_det.reflect(x_det.nameStr(), x_det.id(), sd);
  Volume     vol     = refl.second;
  DetElement sdet    = refl.first;

  if ( !x_det.visStr().empty() )
    vol.setVisAttributes(description, x_det.visStr());
  if ( !x_det.limitsStr().empty() )
    vol.setLimitSet(description, x_det.limitsStr());
  if ( !x_det.regionStr().empty() )
    vol.setRegion(description, x_det.regionStr());
  if ( x_det.isSensitive() )   {
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    sd.setType(sd_typ.typeStr());
  }
  PlacedVolume pv;
  RotationZYX  rot3D;
  Position     tr3D;
  Transform3D  transform3D;
  Volume       mother = description.pickMotherVolume(sdet);
  if ( x_pos )   {
    tr3D = Position(x_pos.x(0),x_pos.y(0),x_pos.z(0));
  }
  if ( x_rot )   {
    rot3D = RotationZYX(x_rot.z(0),x_rot.y(0),x_rot.x(0));
  }
  if ( !x_pos && !x_rot )   {
    auto ref_pv = ref_det.placement();
    matrix::_decompose(ref_pv.matrix(), tr3D, rot3D);
    tr3D = tr3D * (-1.0 / dd4hep::mm);
  }
  if ( x_refl && ::toupper(x_refl.attr<string>(_U(type))[0]) == 'Z' )
    transform3D = Transform3D(Rotation3D( 1., 0., 0., 0.,  1., 0., 0., 0., -1.) * rot3D, tr3D);
  else if ( x_refl && ::toupper(x_refl.attr<string>(_U(type))[0]) == 'Y' )
    transform3D = Transform3D(Rotation3D( 1., 0., 0., 0., -1., 0., 0., 0.,  1.) * rot3D, tr3D);
  else if ( x_refl && ::toupper(x_refl.attr<string>(_U(type))[0]) == 'X' )
    transform3D = Transform3D(Rotation3D(-1., 0., 0., 0.,  1., 0., 0., 0.,  1.) * rot3D, tr3D);
  else  // Z is default
    transform3D = Transform3D(Rotation3D( 1., 0., 0., 0.,  1., 0., 0., 0., -1.) * rot3D, tr3D);
  pv = mother.placeVolume(vol, transform3D);
  printout(INFO,"ReflectedDet","Transform3D placement at pos: %f %f %f rot: %f %f %f",
           tr3D.X(),tr3D.Y(),tr3D.Z(), rot3D.Phi(),rot3D.Theta(),rot3D.Psi());

  if ( x_det.hasAttr(_U(id)) )  {
    pv.addPhysVolID("system",x_det.id());
  }
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_ReflectedDetector,create_element)
