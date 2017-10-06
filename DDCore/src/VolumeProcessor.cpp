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
#include "DD4hep/Printout.h"
#include "DD4hep/VolumeProcessor.h"

using namespace dd4hep;

/// Default destructor
PlacedVolumeProcessor::~PlacedVolumeProcessor()   {
}

/// Callback to output PlacedVolume information of an entire DetElement
int PlacedVolumeProcessor::process(PlacedVolume pv, int level, bool recursive)  const  {
  if ( pv.isValid() )  {
    int ret = (*this)(pv, level);
    TGeoNode* node = pv.ptr();
    if ( recursive )  {
      for (Int_t idau = 0, ndau = node->GetNdaughters(); idau < ndau; ++idau) {
        PlacedVolume placement(node->GetDaughter(idau));
        if ( placement.data() ) {
          ret += process(placement,level+1,recursive);
        }
      }
    }
    return ret;
  }
  except("PlacedVolume","Cannot process an invalid PlacedVolume element");
  return 0;
}
