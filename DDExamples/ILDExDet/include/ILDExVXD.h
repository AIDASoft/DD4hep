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
    ILDExVXD(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id)
      : Geometry::DetElement(lcdd,name,type,id) {}
  };
}
#endif // DD4hep_ILDEXVXD_H
