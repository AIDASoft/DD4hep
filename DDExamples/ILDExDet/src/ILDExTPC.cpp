#include "ILDExTPC.h"
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  ILDExTPC::ILDExTPC(const LCDD&, const string& name, const string& type, int id)
  {
    Value<TNamed,Object>* p = new Value<TNamed,Object>();
    assign(p,name, type);
    p->pressure = 12345.0;
    p->id = id;
  }
  
  double ILDExTPC::getVolume() const {
    return 999.;
  }
  double ILDExTPC::getWeight() const {
    return 888.;
  }
  
  void ILDExTPC::setInnerWall(Ref_t obj) {
    data<Object>()->outerWall = obj;
  }
  
  void ILDExTPC::setOuterWall(Ref_t obj) {
    data<Object>()->innerWall = obj;
  }
  
  void ILDExTPC::setGasVolume(Ref_t obj) {
    data<Object>()->gas = obj;
  }

  void ILDExTPC::setNModules(int n) {
    data<Object>()->nmodules = n;
  }
  int ILDExTPC::getNModules() const {
    return data<Object>()->nmodules;
  }
  void ILDExTPC::setDriftLength(double d) {
    data<Object>()->driftlength = d;
  }
  double ILDExTPC::getDriftLength() const {
    return data<Object>()->driftlength;
  }
  
}
