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

// Framework includes
#include "DD4hep/DetFactoryHelper.h"

using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_dim_t x_det = e;  
  DetElement det(x_det.nameStr(),x_det.id());

  Volume node(       "node",        Box(170, 70, 70), description.air());
  Volume otherParent("otherparent", Box( 50, 50, 50), description.air());
  Volume otherNode(  "othernode",   Box(100, 50, 50), description.material("Iron"));
  Volume parent(     "parent",      Box(90,  40, 40), description.material("Iron"));
  Volume container(  "container",   Box(30,  30, 30), description.air());
  Volume child(      "child",       Box(20,  20, 20), description.material("Iron"));

  sens.setType("tracker");
  
  node.setVisAttributes(        description, x_det.visStr());
  otherParent.setVisAttributes( description, "Shape0_vis");
  otherNode.setVisAttributes(   description, "Shape1_vis");
  parent.setVisAttributes(      description, "Shape2_vis");
  container.setVisAttributes(   description, "Shape3_vis");
  child.setVisAttributes(       description, "Shape4_vis");
  child.setSensitiveDetector(sens);
  
  auto pv = container.placeVolume( child );
  pv.addPhysVolID( "child", 0);
  pv = parent.placeVolume( container, Position( 45, 0, 0) );
  pv.addPhysVolID( "container", 0);
  pv = parent.placeVolume( container, Position(-45, 0, 0) );
  pv.addPhysVolID( "container", 1);
  pv = otherNode.placeVolume( parent );
  pv.addPhysVolID( "parent", 0);
  pv = node.placeVolume( otherNode,   Position( 60, 0, 0) );
  pv.addPhysVolID( "node", 0);

  pv = otherParent.placeVolume( container );
  pv.addPhysVolID( "container",3);
  pv.addPhysVolID( "parent", 1);
  pv = node.placeVolume( otherParent, Position(-110, 0, 0) );
  pv.addPhysVolID( "node", 1);

  // Place the calo inside the world
  auto mother = description.pickMotherVolume(det);
  pv = mother.placeVolume(node);
  pv.addPhysVolID( "system",x_det.id() );
  det.setPlacement(pv);
  return det;
}

DECLARE_DETELEMENT(Issue_786,create_detector)
