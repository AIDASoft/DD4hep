// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : A.Muennich
//
//====================================================================

#include "DD4hep/Detector.h"
//#include "DDTPCPadRowLayout.h"

namespace DD4hep {
  
   
  struct DDTPCModule : public Geometry::DetElement {
    typedef Geometry::Ref_t Ref_t;
    DDTPCModule(const Ref_t& e) : Geometry::DetElement(e) {}
    DDTPCModule(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id);
    int getNPads();
   
  };
}
