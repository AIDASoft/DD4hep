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

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_detector(Detector& description, xml_h e, Ref_t sens)  {
  xml_det_t  x_det = e;
  string     name  = x_det.nameStr();
  DetElement sdet (name,x_det.id());
  Material   mat  (description.material(x_det.materialStr()));
  vector<double> rmin,rmax,z;
  int num = 0;

  for(xml_coll_t c(e,_U(zplane)); c; ++c, ++num)  {
    xml_comp_t dim(c);
    rmin.push_back(dim.rmin());
    rmax.push_back(dim.rmax());
    z.push_back(dim.z()); //Dropped division by 2 in z. Half length not needed
  }
  if ( num < 2 )  {
    throw runtime_error("PolyCone["+name+"]> Not enough Z planes. minimum is 2!");
  }
  Polycone   cone  (0,2*M_PI,rmin,rmax,z);
  Volume     det_vol(name, cone, mat);
  PlacedVolume pv = description.pickMotherVolume(sdet).placeVolume(det_vol);

  sdet.setPlacement(pv);
  det_vol.setVisAttributes(description, x_det.visStr());
  det_vol.setLimitSet(description, x_det.limitsStr());
  det_vol.setRegion(description, x_det.regionStr());
  if ( x_det.isSensitive() )   {
    SensitiveDetector sd = sens;
    xml_dim_t sd_typ = x_det.child(_U(sensitive));
    det_vol.setSensitiveDetector(sens);
    sd.setType(sd_typ.typeStr());
  }

  if ( x_det.hasAttr(_U(id)) )  {
    int det_id = x_det.id();
    pv.addPhysVolID("system",det_id);
  }
  return sdet;
}

DECLARE_DETELEMENT(dd4hep_PolyconeSupport,create_detector)
DECLARE_DEPRECATED_DETELEMENT(PolyconeSupport,create_detector)
