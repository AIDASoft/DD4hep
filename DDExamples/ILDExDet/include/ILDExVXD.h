// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4hep_ILDEXVXD_H
#define DD4hep_ILDEXVXD_H

#include "DD4hep/Detector.h"

namespace DD4hep {
  struct ILDExVXD : public Geometry::DetElement {
    ILDExVXD(const Geometry::Ref_t& e) : Geometry::DetElement(e) {}
    ILDExVXD(const std::string& name, const std::string& type, int id)
      : Geometry::DetElement(name,type,id) {}
  };
}
#endif // DD4hep_ILDEXVXD_H
