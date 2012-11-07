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
  
 
  struct ILDExTPC : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;
    ILDExTPC(const Ref_t& e) : Geometry::DetElement(e) {}
    ILDExTPC(const std::string& name, const std::string& type, int id);
    void setInnerWall(Ref_t obj);
    void setOuterWall(Ref_t obj);
    void setGasVolume(Ref_t obj);
    void setCathode(Ref_t obj);
    void setEndPlate(Ref_t obj, int i);
  };
}
