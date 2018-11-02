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

  builder.debug = x_dbg != 0;
  builder.buildShapes(x_det);
  builder.buildShapes(x_env);
  builder.buildVolumes(x_det);
  builder.buildVolumes(x_env);

  // Need to keep these alive as long as the volumebuilder lives
  map<string, xml::DocumentHolder*> docs;
  for( xml_coll_t c(x_det,_U(include)); c; ++c )   {
    string ref = c.attr<string>(_U(ref));
    docs[ref]  = new xml::DocumentHolder(xml::DocumentHandler().load(e, c.attr_value(_U(ref))));
    xml_h vols = docs[ref]->root();
    builder.buildShapes(vols);
    builder.buildVolumes(vols);
  }
  for(auto& d : docs) delete d.second;
  
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
  if ( x_env.hasAttr(_U(vis)) )  {
    assembly.setVisAttributes(description, x_env.visStr());
  }
  if ( x_det.hasAttr(_U(sensitive)) )  {
    sens.setType(x_det.attr<string>(_U(sensitive)));
  }
  if ( x_env.hasAttr(_U(name)) )   {
    assembly->SetName(x_env.nameStr().c_str());
  }
  builder.placeDaughters(builder.detector, assembly, x_env);
  builder.placeDaughters(builder.detector, assembly, x_det);
  x_pos = x_env.child(_U(position),false);
  x_rot = x_env.child(_U(rotation),false);
  x_tr  = x_env.child(_U(transformation),false);
  builder.placeDetector(assembly, (x_pos || x_rot || x_tr) ? x_env : x_det);
  printout(builder.debug ? ALWAYS : DEBUG, "VolumeBuilder",
           "+++ Created subdetector instance %s",builder.name.c_str());
  return builder.detector;
}
DECLARE_DETELEMENT(DD4hep_VolumeAssembly,create_element)
