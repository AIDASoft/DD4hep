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
  xml_dim_t  pos     = x_det.child(_U(position),false);
  xml_dim_t  rot     = x_det.child(_U(rotation),false);
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
  Volume       mother = description.pickMotherVolume(sdet);
  if ( pos && rot )   {
    pv = mother.placeVolume(vol,Transform3D(RotationZYX(rot.z(0),rot.y(0),rot.x(0)),Position(pos.x(0),pos.y(0),pos.z(0))));
    printout(INFO,"ReflectedDet","Transform3D placement at pos: %f %f %f rot: %f %f %f",
             pos.x(0),pos.y(0),pos.z(0), rot.x(0),rot.y(0),rot.z(0));
  }
  else if ( rot )   {
    printout(INFO,"ReflectedDet","Rotation placement at %f %f %f",rot.x(0),rot.y(0),rot.z(0));
    pv = mother.placeVolume(vol,RotationZYX(rot.z(0),rot.y(0),rot.x(0)));
  }
  else if ( pos )   {
    printout(INFO,"ReflectedDet","Positional placing at %f %f %f",pos.x(0),pos.y(0),pos.z(0));
    pv = mother.placeVolume(vol,Position(pos.x(0),pos.y(0),pos.z(0)));
  }
  else   {
    auto ref_pv = ref_det.placement();
    RotationZYX rot3D;
    Position    tr3D;
    matrix::_decompose(ref_pv.matrix(), tr3D, rot3D);
    tr3D = tr3D * (-1.0 / dd4hep::mm);
    rot3D = rot3D * RotationZ();
    pv = mother.placeVolume(vol, Transform3D(rot3D, tr3D));
    printout(INFO,"ReflectedDet","Transform3D placement at pos: %f %f %f rot: %f %f %f",
             tr3D.X(),tr3D.Y(),tr3D.Z(), rot3D.Phi(),rot3D.Theta(),rot3D.Psi());
  }
  if ( x_det.hasAttr(_U(id)) )  {
    pv.addPhysVolID("system",x_det.id());
  }
  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(DD4hep_ReflectedDetector,create_element)
