#include "DDTPCEndPlate.h"
#include "TPCEndPlateData.h"
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  DDTPCEndPlate::DDTPCEndPlate(const LCDD&, const string& name, const string& type, int id)
  {
    Value<TNamed,TPCEndPlateData>* p = new Value<TNamed,TPCEndPlateData>();
    assign(p,name, type);

  }
  

  int DDTPCEndPlate::getNModules() {
    return children().size();
  }
  

}
