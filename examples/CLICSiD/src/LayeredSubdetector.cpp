#include "LayeredSubdetector.h"

namespace DD4hep {
namespace Geometry {

LayeredSubdetector::LayeredSubdetector(const DetElement& e) :
		DetElement(e) {
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

} /* namespace Geometry */
} /* namespace DD4hep */
