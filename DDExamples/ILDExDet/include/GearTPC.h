#include "SubDetector.h"

namespace DetDesc {

  struct TPCData;

  struct GearTPC : public Geometry::Subdetector {
    typedef TPCData Object;
    GearTPC(const RefElement& e);
    double innerRadius() const;
    double outerRadius() const;
    double pressure() const;
  };
}
