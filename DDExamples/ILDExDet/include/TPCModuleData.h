// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================

#include "DD4hep/Detector.h"

namespace DD4hep {

  struct TPCModuleData   {
    double rowHeight;
    double padWidth;
    double padHeight;
    double padGap;
    int nRows;
    int nPads;
  };
}
