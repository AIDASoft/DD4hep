#include "DDRec/VertexBarrelDetector.h"

using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec;

/// Constructor from handle
VertexBarrelDetector::VertexBarrelDetector(const DetElement& e) 
  : BarrelDetector(e), LayeredSubdetector(e) 
{
}

/// Standard destructor
VertexBarrelDetector::~VertexBarrelDetector() {
}
