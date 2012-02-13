// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Detector.h"

namespace DD4hep {
  
  struct TPCData;
  
  struct GearTPC : public Geometry::DetElement {
    typedef TPCData Object;
    GearTPC(const Geometry::Ref_t& e);
    double innerRadius() const;
    double outerRadius() const;
    double pressure() const;
  };
}
