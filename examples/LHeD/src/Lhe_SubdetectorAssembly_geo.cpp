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
//  mod.:        P.Kostka LHeD (to be done: asym.detector placement in z)
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "XML/Utilities.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t)  {
  xml_det_t  x_det  (e);
  string     det_name = x_det.nameStr();
  DetElement sdet(det_name, x_det.id());
  Volume     vol;

  bool useRot = x_det.hasChild(_U(rotation));
  bool usePos = x_det.hasChild(_U(position));
  Position    pos;
  RotationZYX rot;

  sdet.setType("compound");
  xml::setDetectorTypeFlag( e, sdet ) ;

  if( usePos ) {
    pos = Position(x_det.position().x(), x_det.position().y(), x_det.position().z());
  }
  if( useRot ) {
    rot = RotationZYX(x_det.rotation().x(), x_det.rotation().y(), x_det.rotation().z());
  }

  if ( x_det.hasChild(_U(shape)) )  {
    xml_comp_t x_shape = x_det.child(_U(shape));
    string     type  = x_shape.typeStr();
    Solid      solid = xml::createShape(description, type, x_shape);
    Material   mat   = description.material(x_shape.materialStr());
    printout(DEBUG,det_name,"+++ Creating detector assembly with shape of type:%s",type.c_str());
    vol = Volume(det_name,solid,mat);
  }
  else  {
    printout(DEBUG,det_name,"+++ Creating detector assembly without shape");
    vol = Assembly(det_name);
  }

  for(xml_coll_t c(x_det,_U(composite)); c; ++c)  {
    xml_dim_t component = c;
    string nam = component.nameStr();
    description.declareMotherVolume(nam, vol);
  }

  vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  Volume mother = description.pickMotherVolume(sdet);
  PlacedVolume pv;
  if( useRot && usePos ){
    pv =  mother.placeVolume(vol, Transform3D(rot, pos));
  } else if( useRot ){
    pv =  mother.placeVolume(vol, rot);
  } else if( usePos ){
    pv =  mother.placeVolume(vol, pos);
  } else {
    pv = mother.placeVolume(vol);
  }

  sdet.setPlacement(pv);
  return sdet;
}

DECLARE_DETELEMENT(Lhe_SubdetectorAssembly,create_element)
