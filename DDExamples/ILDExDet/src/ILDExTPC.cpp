#include "ILDExTPC.h"
#include "TPCData.h"
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  ILDExTPC::ILDExTPC(const LCDD&, const string& name, const string& type, int id)
  {
    Value<TNamed,TPCData>* p = new Value<TNamed,TPCData>();
    assign(p,name, type);
    p->id = id;
  }
  

  void ILDExTPC::setInnerWall(Ref_t obj) {
    data<TPCData>()->innerWall= obj;
  }
  
  void ILDExTPC::setOuterWall(Ref_t obj) {
    data<TPCData>()->outerWall = obj;
  }
  
  void ILDExTPC::setGasVolume(Ref_t obj) {
    data<TPCData>()->gas = obj;
  }

  void ILDExTPC::setEndPlate(Ref_t obj) {
    data<TPCData>()->endplate = obj;
  }

  int ILDExTPC::getNModules() const {
    return data<TPCData>()->nmodules;
  }
  void ILDExTPC::setDriftLength(double d) {
    data<TPCData>()->driftlength = d;
  }
  double ILDExTPC::getDriftLength() const {
    return data<TPCData>()->driftlength;
  }
  
}
