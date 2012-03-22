#include "DDTPCModule.h"
#include "TPCModuleData.h"
#include <iostream>

using namespace std;

namespace DD4hep {
  
  using namespace Geometry;
  
  DDTPCModule::DDTPCModule(const LCDD&, const string& name, const string& type, int id)
  {
    Value<TNamed,TPCModuleData>* p = new Value<TNamed,TPCModuleData>();
    assign(p,name, type);

  }
  
 
//   int DDTPCModule::getNPads() {
//     return  data<TPCModuleData>()->padRowLayout.getNPads();
//   }
  

}
