/*
 * CellIDDecoderFactory.h
 *
 * This class would be obsolete if Geometry::Segmentation would directly use the DDSegmentation classes
 *
 *  Created on: Sep 16, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CELLIDDECODERFACTORY_H_
#define CELLIDDECODERFACTORY_H_

#include "DD4hep/LCDD.h"
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/CartesianGridXY.h"

#include <iostream>
#include <string>

namespace DD4hep {
namespace DDSegmentation {

namespace CellIDDecoderFactory {
CellIDDecoder getCellIDDecoder(const std::string& collectionName) {
	Geometry::LCDD& lcdd = Geometry::LCDD::getInstance();
	Geometry::VolumeManager volManager = lcdd.volumeManager();
	Geometry::Readout readout = lcdd.readout(collectionName);
	Geometry::Segmentation s = readout.segmentation();
	Segmentation* segmentation;
	std::string type = s.type();
	if (type == "grid_xyz") {
		Geometry::GridXYZ g = (Geometry::GridXY) s;
		segmentation = new CartesianGridXY(readout.idSpec().decoder(), g.getGridSizeX(), g.getGridSizeY());
	} else {
	    throw std::runtime_error("The segmentation type "+type+" is not supported by CellIDDecoderFactory.");
	}
	return CellIDDecoder(volManager, segmentation);
}
}

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* CELLIDDECODERFACTORY_H_ */
