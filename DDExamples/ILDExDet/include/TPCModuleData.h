// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================
#ifndef DD4HEP_ILDEXDET_TPCMODULEDATA_H
#define DD4HEP_ILDEXDET_TPCMODULEDATA_H

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
#endif DD4HEP_ILDEXDET_TPCMODULEDATA_H
