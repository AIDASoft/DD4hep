// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Detector.h"
#include <vector>

namespace DD4hep {
  
  
  struct TPCData : public Geometry::DetElement::Object {
    typedef Geometry::Ref_t Ref_t;
    Ref_t outerWall;
    Ref_t innerWall;
    Ref_t gas;
    Ref_t cathode;
    Ref_t endplate;
    Ref_t endplate2;
    int nmodules;
    double driftlength;
  };
}
