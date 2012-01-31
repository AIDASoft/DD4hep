#include "ILDExTPC.h"
#include "Internals.h"
#include <iostream>

using namespace std;

namespace DetDesc {

  using namespace Geometry;

  ILDExTPC::ILDExTPC(const Document&, const string& name, const string& type, int id)
    : Subdetector(new Value<TNamed,Object>()) 
  {
    Value<TNamed,Object>* p = (Value<TNamed,Object>*)m_element.ptr();
    p->SetName(name.c_str());
    p->SetTitle(type.c_str());
    p->pressure = 12345.0;

    setAttr(Attr_id,id);
  }

  double ILDExTPC::getVolume() const {
    return 999.;
  }
  double ILDExTPC::getWeight() const {
    return 888.;
  }

  void ILDExTPC::setInnerWall(RefElement obj) {
    setAttr(outerWall,obj);
  }

  void ILDExTPC::setOuterWall(RefElement obj) {
    setAttr(innerWall,obj);
  }

  void ILDExTPC::setGasVolume(RefElement obj) {
    setAttr(gas,obj);
  }

}
