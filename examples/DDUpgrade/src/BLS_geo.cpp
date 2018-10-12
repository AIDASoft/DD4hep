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
using namespace dd4hep::xml::tools;

static Ref_t create_element(Detector& description, xml_h e, Ref_t sens_det)  {
  xml_det_t     x_det = e;
  VolumeBuilder builder(description, x_det, sens_det);
  DetElement    bls_det = builder.detector;
  Assembly      bls_vol(string("lv")+bls_det.name());

  builder.buildTransformations(e);
  builder.buildVolumes(e);
  builder.placeDaughters(bls_det, bls_vol, e);
  /// Now we have to attach the volume ids for the sensitive volumes for DDG4
  const auto& children = bls_det.children();
  for ( const auto& c : children )  {
    PlacedVolume pv = c.second.placement();
    pv.addPhysVolID("module",c.second.id());
    for (Int_t idau = 0, ndau = pv->GetNdaughters(); idau < ndau; ++idau) {
      PlacedVolume p(pv->GetDaughter(idau));
      if ( p.volIDs().empty() && p.volume().isSensitive() )   {
        p.addPhysVolID("sensor",1);
      }
    }
  }
  PlacedVolume pv = builder.placeDetector(bls_vol);
  pv.addPhysVolID("system", x_det.id());
  return bls_det;
}
// first argument is the type from the xml file
DECLARE_DETELEMENT(LHCb_Bls,create_element)

