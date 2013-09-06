// $Id$
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
    Ref_t gasVolume;
    Ref_t cathode;
    //positive endplate
    Ref_t endplate;
    //negative endplate
    Ref_t endplate2;
  };
}
