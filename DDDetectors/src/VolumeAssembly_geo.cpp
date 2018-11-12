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
#include "XML/VolumeBuilder.h"
#include "XML/Utilities.h"
#include <memory>

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector sens)  {
  //Builder  b(description);
  //return b.create(e, sens);
  xml_comp_t  x_det(e);
  xml_comp_t  x_env = e.child(_U(envelope));
  xml_comp_t  x_shp = x_env.child(_U(shape),false);
  xml_h       x_dbg = e.child(_U(debug),false);
  xml_h       x_pos, x_rot, x_tr;
  Volume      assembly;
  xml::tools::VolumeBuilder builder(description, e, sens);

  builder.debug = x_dbg != 0 || true;
  builder.load(x_det, "include");
  builder.buildShapes(x_det);
  builder.buildShapes(x_env);
  builder.buildVolumes(x_det);
  builder.buildVolumes(x_env);
  
  // Now we build the envelope
  if ( !x_shp ) x_shp = x_env;
  if ( x_shp.typeStr() == "Assembly" )  {
    assembly = Assembly("lv"+builder.name);
  }
  else  {
    Material mat   = description.material(x_env.materialStr());
    Solid    solid = xml::createShape(description, x_shp.typeStr(), x_shp);
    assembly = Volume("lv"+builder.name, solid, mat);
  }
  /// Set generic associations
  assembly.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
  /// If specified more direct: use these ones.
  if ( x_env.hasAttr(_U(vis)) )
    assembly.setVisAttributes(description, x_env.visStr());

  if ( x_env.hasAttr(_U(region)) )
    assembly.setRegion(description, x_env.regionStr());

  if ( x_env.hasAttr(_U(limits)) )
    assembly.setLimitSet(description, x_env.limitsStr());

  if ( x_det.hasAttr(_U(sensitive)) )
    sens.setType(x_det.attr<string>(_U(sensitive)));
  else
    builder.detector.setType("compound");

  if ( x_env.hasAttr(_U(name)) )
    assembly->SetName(x_env.nameStr().c_str());

  builder.placeDaughters(builder.detector, assembly, x_env);
  builder.placeDaughters(builder.detector, assembly, x_det);
  x_pos = x_env.child(_U(position),false);
  x_rot = x_env.child(_U(rotation),false);
  x_tr  = x_env.child(_U(transformation),false);
  builder.placeDetector(assembly, (x_pos || x_rot || x_tr) ? x_env : x_det);
  printout(builder.debug ? ALWAYS : DEBUG, "VolumeBuilder",
           "+++ Created subdetector instance %s vis:",
           builder.name.c_str(), x_det.visStr().c_str());
  return builder.detector;
}
DECLARE_DETELEMENT(DD4hep_VolumeAssembly,create_element)
