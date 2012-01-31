#include "SubDetector.h"

namespace DetDesc {
  
  struct TPCData : public Geometry::Subdetector::Object {
    Geometry::RefElement outerWall;
    Geometry::RefElement innerWall;
    Geometry::RefElement gas;
    double pressure;
  };

  struct ILDExTPC : public Geometry::Subdetector {
    typedef TPCData Object;
    ILDExTPC(const Geometry::RefElement& e) : Geometry::Subdetector(e) {}
    ILDExTPC(const Geometry::Document& doc, const std::string& name, const std::string& type, int id);
    void setInnerWall(Geometry::RefElement obj);
    void setOuterWall(Geometry::RefElement obj);
    void setGasVolume(Geometry::RefElement obj);
    double getVolume() const;
    double getWeight() const;
  };
}
