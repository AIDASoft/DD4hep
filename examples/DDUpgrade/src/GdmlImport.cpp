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
#include "TGeoManager.h"
#include "TGDMLParse.h"
#include "TUri.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, Ref_t /* sens_det */)  {
  xml_det_t   x_det = e;
  int         id    = x_det.hasAttr(_U(id)) ? x_det.id() : 0;
  xml_dim_t   x_pos  (x_det.child(_U(position),false));
  xml_dim_t   x_rot  (x_det.child(_U(rotation),false));
  xml_dim_t   x_gdml (x_det.child(_U(gdmlFile)));
  xml_dim_t   x_reg  (x_det.child(_U(region)));
  string      name   = x_det.nameStr();
  string      region = x_reg.nameStr();
  string      input = x_gdml.attr<string>(_U(ref));
  DetElement  det_region = description.detector(region);
  TGDMLParse parser;
  TUri uri(input.c_str());

  input = uri.GetRelativePart();
  if ( !det_region.isValid() )  {
    except(name,"+++ Cannot access detector region: %s",region.c_str());
  }  
  DetElement  sdet(name, id);
  Volume volume = parser.GDMLReadFile(input.c_str());
  if ( !volume.isValid() )   {
    except("ROOTGDMLParse","+++ Failed to parse GDML file:%s",input.c_str());
  }
  volume.import(); // We require the extensions in dd4hep.
  printout(INFO,"ROOTGDMLParse","+++ Attach GDML volume %s", volume.name());
  Volume mother = det_region.volume();
  PlacedVolume pv;

  if ( x_pos && x_rot )   {
    Rotation3D rot(RotationZYX(x_rot.z(),x_rot.y(),x_rot.x()));
    Transform3D transform(rot,Position(x_pos.x(),x_pos.y(),x_pos.z()));
    pv = mother.placeVolume(volume,transform);
  }
  else if ( x_rot )  {
    Rotation3D rot(RotationZYX(x_rot.z(),x_rot.y(),x_rot.x()));
    Transform3D transform(rot,Position(0,0,0));
    pv = mother.placeVolume(volume,transform);
  }
  else if ( x_pos )   {
    pv = mother.placeVolume(volume,Position(x_pos.x(),x_pos.y(),x_pos.z()));
  }
  else  {
    pv = mother.placeVolume(volume);
  }
  volume.setVisAttributes(description, x_det.visStr());
  volume.setLimitSet(description, x_det.limitsStr());
  volume.setRegion(description, x_det.regionStr());
  if ( id != 0 )  {
    pv.addPhysVolID("system", id);
  }
  sdet.setPlacement(pv);
  return sdet;
}

// first argument is the type from the xml file
DECLARE_DETELEMENT(DD4hep_GdmlImport,create_element)
