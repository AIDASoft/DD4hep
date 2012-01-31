#include "ILDExTPC.h"
#include "GearTPC.h"
#include "Volumes.h"
#include "Shapes.h"
#include "Internals.h"
#include "TGeoTube.h"
#include <typeinfo>
#include <string>

using namespace std;

namespace DetDesc {
  using namespace Geometry;

  GearTPC::GearTPC(const RefElement& e) : Geometry::Subdetector(e) {
    if ( dynamic_cast<Value<TNamed,Object>* >(e.ptr()) == 0 )  {
      const type_info& from = typeid(*(e.ptr()));
      const type_info& to   = typeid(Value<TNamed,Object>);
      string msg = "Wrong assingment from ";
      msg += from.name();
      msg += " to ";
      msg += to.name();
      msg += " not possible!!";
      throw std::runtime_error(msg);
    }
  }

  double GearTPC::innerRadius() const {
    Subdetector gas   = getAttr(gas);
    TGeoTube&   tube  = (TGeoTube&)gas.volume().solid().shape();
    return tube.GetRmin();
  }
  double GearTPC::outerRadius() const {
    Subdetector gas   = getAttr(gas);
    TGeoTube&   tube  = (TGeoTube&)gas.volume().solid().shape();
    return tube.GetRmax();
  }
  double GearTPC::pressure() const {
    return getAttr(pressure);
  }
}
