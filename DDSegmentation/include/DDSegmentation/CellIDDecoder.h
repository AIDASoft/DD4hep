/*
 * CellIDDecoder.h
 * This is an example of mixing the segmentation interface with the volume manager. Could move into Readout class.
 *  Created on: Sep 16, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef CELLIDDECODER_H_
#define CELLIDDECODER_H_

#include "DD4hep/VolumeManager.h"
#include "DD4hep/Objects.h"
#include "DD4hep/TGeoUnits.h"
#include "DDSegmentation/Segmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class CellIDDecoder {
public:
	CellIDDecoder(const Geometry::VolumeManager& volManager, const Segmentation* segmentation) : _volManager(volManager), _segmentation(segmentation) {

	}

	virtual ~CellIDDecoder() {
		delete _segmentation;
	}

	Geometry::PlacedVolume getPlacement(const long& cellID) const {
		return _volManager.lookupPlacement(cellID);
	}

	Geometry::DetElement getSubDetector(const long& cellID) const {
		return _volManager.lookupDetector(cellID);
	}

	Geometry::DetElement getDetectorElement(const long& cellID) const {
		return _volManager.lookupDetElement(cellID);
	}

	Geometry::Position getPosition(const long64& cellID) const {
		double global[3] = {0., 0., 0.};
		_volManager.worldTransformation(cellID).LocalToMaster(&(_segmentation->getLocalPosition(cellID))[0], global);
		return Geometry::Position(global[0]/tgeo::mm, global[1]/tgeo::mm, global[2]/tgeo::mm);
	}

	const TGeoMatrix& getWorldTransformation(const long& cellID) const {
		return _volManager.worldTransformation(cellID);
	}

	Geometry::Position getLocalPosition(const long64& cellID) const {
		std::vector<double> v = _segmentation->getLocalPosition(cellID);
		return Geometry::Position(v[0], v[1], v[2]);
	}

	long64 getCellID(double x, double y, double z) const {
		return _segmentation->getCellID(x, y, z);
	}

	std::string fieldDescription() const {
		return _segmentation->fieldDescription();
	}

protected:
	const Segmentation* _segmentation;
	const Geometry::VolumeManager _volManager;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* CELLIDDECODER_H_ */
