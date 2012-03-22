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
  
  
  struct DDTPCEndPlate : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;
    DDTPCEndPlate(const Ref_t& e) : Geometry::DetElement(e) {}
    DDTPCEndPlate(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id);
    int getNModules();
 };
}
