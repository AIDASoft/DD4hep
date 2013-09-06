// $Id$
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

  //data container used for things not covered by a DetElement
  // no good example at the moment that is not either conditions data or otherwise
  struct TPCModuleData   {
    double padGap;
    bool myFlag;
  };
}
#endif //DD4HEP_ILDEXDET_TPCMODULEDATA_H
