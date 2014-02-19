#include "DDRec/LayeredSubdetector.h"

using namespace DD4hep::Geometry;
using namespace DD4hep::DDRec;

LayeredSubdetector::LayeredSubdetector(const DetElement& e)
 : DetElement(e) 
{
  getExtension();
}

LayeredSubdetector::~LayeredSubdetector() {
}

void LayeredSubdetector::getExtension() {
  layerStack = isValid() ? extension<LayerStack>() : 0;
  if (layerStack == 0) {
    std::cout << "Failed to get the extension" << std::endl;
  }
}
