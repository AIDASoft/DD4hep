/*
 * IDDecoder.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDRec/API/IDDecoder.h"
#include "DDRec/API/Exceptions.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/VolumeManager.h"

namespace DD4hep {
namespace DDRec {

using Geometry::DetElement;
using Geometry::LCDD;
using Geometry::PlacedVolume;
using Geometry::Position;
using Geometry::Readout;
using Geometry::Solid;
using Geometry::VolumeManager;
using Geometry::Volume;
using std::set;

IDDecoder& IDDecoder::getInstance() {
	static IDDecoder idd;
	return idd;
}

/// Default constructor
IDDecoder::IDDecoder() {
	LCDD& lcdd = LCDD::getInstance();
	_volumeManager = lcdd.volumeManager();
	if (not _volumeManager.isValid()) {
		_volumeManager = VolumeManager(lcdd, "volman", lcdd.world(), Readout(), VolumeManager::TREE);
	}
	_tgeoMgr = lcdd.world().volume()->GetGeoManager();
}

/**
 * Returns the cell ID from the local position in the given volume ID.
 */
CellID IDDecoder::cellIDFromLocal(const Position& local, const VolumeID volumeID) const {
	double l[3];
	double g[3];
	local.GetCoordinates(l);
	// FIXME: direct lookup of transformations seems to be broken
	//const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(volumeID);
	DetElement det = this->detectorElement(volumeID);
	const TGeoMatrix& localToGlobal = det.worldTransformation();
	localToGlobal.LocalToMaster(l, g);
	Position global(g[0], g[1], g[2]);
	return this->findReadout(det).segmentation().cellID(local, global, volumeID);
}

/**
 * Returns the global cell ID from a given global position
 */
CellID IDDecoder::cellID(const Position& global) const {
	VolumeID volID = volumeID(global);
	double l[3];
	double g[3];
	global.GetCoordinates(g);
	// FIXME: direct lookup of transformations seems to be broken
	//const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(volID);
	DetElement det = this->detectorElement(volID);
	const TGeoMatrix& localToGlobal = det.worldTransformation();
	localToGlobal.MasterToLocal(g, l);
	Position local(l[0], l[1], l[2]);
	return this->findReadout(det).segmentation().cellID(local, global, volID);
}

/**
 * Returns the global position from a given cell ID
 */
Position IDDecoder::position(const CellID& cellID) const {
	double l[3];
	double g[3];
	DetElement det = this->detectorElement(cellID);
	Position local = this->findReadout(det).segmentation().position(cellID);
	local.GetCoordinates(l);
	// FIXME: direct lookup of transformations seems to be broken
	//const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(cellID);
	const TGeoMatrix& localToGlobal = det.worldTransformation();
	localToGlobal.LocalToMaster(l, g);
	return Position(g[0], g[1], g[2]);
}

/*
 * Returns the local position from a given cell ID
 */
Position IDDecoder::localPosition(const CellID& cellID) const {
	DetElement det = this->detectorElement(cellID);
	return this->findReadout(det).segmentation().position(cellID);
}

/*
 * Returns the volume ID of a given cell ID
 */
VolumeID IDDecoder::volumeID(const CellID& cellID) const {
	DetElement det = this->detectorElement(cellID);
	return this->findReadout(det).segmentation()->volumeID(cellID);
}

/*
 * Returns the volume ID of a given global position
 */
VolumeID IDDecoder::volumeID(const Position& position) const {
	DetElement det = this->detectorElement(position);
	return det.volumeID();
}

/*
 * Returns the placement for a given cell ID
 */
PlacedVolume IDDecoder::placement(const CellID& cellID) const {
	return _volumeManager.lookupPlacement(cellID);
}

/*
 * Returns the placement for a given global position
 */
PlacedVolume IDDecoder::placement(const Position& position) const {
	return placement(volumeID(position));
}

/*
 * Returns the subdetector for a given cell ID
 */
DetElement IDDecoder::subDetector(const CellID& cellID) const {
	return _volumeManager.lookupDetector(cellID);
}

/*
 * Returns the subdetector for a given global position
 */
DetElement IDDecoder::subDetector(const Position& position) const {
	return subDetector(volumeID(position));
}

/*
 * Returns the closest detector element in the hierarchy for a given cell ID
 */
DetElement IDDecoder::detectorElement(const CellID& cellID) const {
	return _volumeManager.lookupDetElement(cellID);
}

/*
 * Returns the closest detector element in the hierarchy for a given global position
 */
DetElement IDDecoder::detectorElement(const Position& position) const {
	DetElement world = Geometry::LCDD::getInstance().world();
	DetElement det = getClosestDaughter(world, position);
	if (not det.isValid()) {
		throw invalid_position("DD4hep::DDRec::IDDecoder::detectorElement", position);
	}
	std::cout << det.name() << std::endl;
	return det;
}

/// Access to the Readout object for a given cell ID
Geometry::Readout IDDecoder::readout(const CellID& cellID) const {
	DetElement det = this->detectorElement(cellID);
	return this->findReadout(det);
}

/// Access to the Readout object for a given global position
Geometry::Readout IDDecoder::readout(const Geometry::Position& global) const {
	DetElement det = this->detectorElement(global);
	return this->findReadout(det);
}

/*
 * Calculates the neighbours of the given cell ID and adds them to the list of neighbours
 */
void IDDecoder::neighbours(const CellID& cellID, set<CellID>& neighbours) const {
	DetElement det = this->detectorElement(cellID);
	this->findReadout(det).segmentation()->neighbours(cellID, neighbours);
}

/*
 * Checks if the given cell IDs are neighbours
 */
bool IDDecoder::areNeighbours(const CellID& cellID, const CellID& otherCellID) const {
	set<CellID> neighbours;
	DetElement det = this->detectorElement(cellID);
	this->findReadout(det).segmentation()->neighbours(cellID, neighbours);
	return neighbours.count(otherCellID) != 0;
}

/// Access to the barrel-endcap flag
IDDecoder::BarrelEndcapFlag IDDecoder::barrelEndcapFlag(const CellID& cellID) const {
	Readout r = this->readout(cellID);
	return BarrelEndcapFlag(r.idSpec().field(this->barrelIdentifier())->value(cellID));
}

/// Access to the layer index
long int IDDecoder::layerIndex(const CellID& cellID) const {
	Readout r = this->readout(cellID);
	return r.idSpec().field(this->layerIdentifier())->value(cellID);
}

/// Access to the system index
long int IDDecoder::systemIndex(const CellID& cellID) const {
	Readout r = this->readout(cellID);
	return r.idSpec().field(this->systemIdentifier())->value(cellID);
}

// helper method to find the corresponding Readout object to a DetElement
Readout IDDecoder::findReadout(const Geometry::DetElement& det) const {

	// first check if top level is a sensitive detector
	if (det.volume().isValid() and det.volume().isSensitive()) {
		Geometry::SensitiveDetector sd = det.volume().sensitiveDetector();
		if (sd.isValid() and sd.readout().isValid()) {
			return sd.readout();
		}
	}

	// check all children recursively for the first valid Readout object
	const DetElement::Children& children = det.children();
	DetElement::Children::const_iterator it = children.begin();
	while (it != children.end()) {
		Readout r = findReadout(it->second);
		if (r.isValid()) {
			return r;
		}
		++it;
	}

	// neither this or any daughter is sensitive
	return Readout();
}

// helper method to get the closest daughter DetElement to the position starting from the given DetElement
DetElement IDDecoder::getClosestDaughter(const DetElement& det, const Position& position) {
	DetElement result;

	// check if we have a shape and see if we are inside
	if (det.volume().isValid() and det.volume().solid().isValid()) {
		double globalPosition[3] = { position.x(), position.y(), position.z() };
		double localPosition[3] = { 0., 0., 0. };
		det.worldTransformation().MasterToLocal(globalPosition, localPosition);
		if (det.volume().solid()->Contains(localPosition)) {
			result = det;
		} else {
			// assuming that any daughter shape would be inside this shape
			return DetElement();
		}
	}

	const DetElement::Children& children = det.children();
	DetElement::Children::const_iterator it = children.begin();
	while (it != children.end()) {
		DetElement daughterDet = getClosestDaughter(it->second, position);
		if (daughterDet.isValid()) {
			result = daughterDet;
			break;
		}
		++it;
	}
	return result;
}

} /* namespace DDRec */
} /* namespace DD4hep */
