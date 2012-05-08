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
  
  
  struct TPCModuleData : public Geometry::DetElement::Object {
    typedef Geometry::Ref_t Ref_t;
    double rowHeight ;
    double padWidth ;
    double padHeight ;
    double padGap ;
    int nRows ;
    int nPads ;
    
  };
}
