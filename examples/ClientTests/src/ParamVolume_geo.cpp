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

namespace  {
  Transform3D get_trafo(xml_dim_t elt)   {
    xml_dim_t  x_pos   = elt.position();
    xml_dim_t  x_rot   = elt.rotation();
    return Transform3D(RotationZYX(x_rot.x(), x_rot.y(), x_rot.z()),
		       Position(x_pos.x(), x_pos.y(), x_pos.z()));
  }
}

static Ref_t create_detector(Detector& description, xml_h e, SensitiveDetector sens)  {
  // XML detector object: DDCore/XML/XMLDetector.h
  xml_dim_t x_det = e;  
  //Create the DetElement for dd4hep
  DetElement det(x_det.nameStr(),x_det.id());

  // XML dimension object: DDCore/XML/XMLDimension.h
  xml_dim_t x_box(x_det.child(_U(box)));
  Volume    envelope_vol(x_det.nameStr()+"_envelope", 
			 Box(x_box.x(), x_box.y(), x_box.z()),
			 description.material(x_box.attr<std::string>(_U(material))));

  // Set envelope volume attributes
  envelope_vol.setAttributes(description,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  xml_comp_t x_param = x_det.child(_U(param));
  Box        box     (x_param.x(), x_param.y(), x_param.z());
  Volume     box_vol (x_det.nameStr()+"_param", box, description.material(x_param.materialStr()));
  xml_dim_t  x_trafo = x_param.transformation();
  xml_dim_t  x_dim_x = x_trafo.child(_U(dim_x));
  Transform3D start, trafo1;
  PlacedVolume pv;

  if ( x_param.hasChild(_U(start)) )   {
    start = get_trafo(x_param.child(_U(start)));
  }

  trafo1 = get_trafo(x_trafo.child(_U(dim_x)));

  if ( x_trafo.hasChild(_U(dim_y)) && x_trafo.hasChild(_U(dim_z)) )    {
    xml_dim_t   x_dim_y = x_trafo.child(_U(dim_y));
    xml_dim_t   x_dim_z = x_trafo.child(_U(dim_z));
    Transform3D trafo2 = get_trafo(x_dim_y);
    Transform3D trafo3 = get_trafo(x_dim_z);
    pv = envelope_vol.paramVolume3D(start, box_vol, 
				    x_dim_x.repeat(), trafo1,
				    x_dim_y.repeat(), trafo2,
				    x_dim_z.repeat(), trafo3);
  }
  else if ( x_trafo.hasChild(_U(dim_y)) )    {
    xml_dim_t   x_dim_y = x_trafo.child(_U(dim_y));
    Transform3D trafo2 = get_trafo(x_dim_y);
    pv = envelope_vol.paramVolume2D(start, box_vol, 
				    x_dim_x.repeat(), trafo1, 
				    x_dim_y.repeat(), trafo2);
  }
  else   {
    pv = envelope_vol.paramVolume1D(start, box_vol, x_dim_x.repeat(), trafo1);
  }
  if ( sens.isValid() )   {
    sens.setType("calorimeter");
    pv.addPhysVolID("volume", 0);
  }
  box_vol.setSensitiveDetector(sens);
  box_vol.setAttributes(description,x_param.regionStr(),x_param.limitsStr(),x_param.visStr());

  det.setAttributes(description,envelope_vol,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());

  // Place the calo inside the world
  xml_dim_t x_pos = x_det.position();
  xml_dim_t x_rot = x_det.rotation();
  auto mother = description.pickMotherVolume(det);
  Transform3D tr(RotationZYX(x_rot.x(), x_rot.y(), x_rot.z()),
		 Position(x_pos.x(), x_pos.y(), x_pos.z()));
  PlacedVolume envelope_plv = mother.placeVolume(envelope_vol, tr);
  envelope_plv.addPhysVolID("system",x_det.id());
  det.setPlacement(envelope_plv);
  return det;
}

DECLARE_DETELEMENT(DD4hep_ParamVolume,create_detector)
