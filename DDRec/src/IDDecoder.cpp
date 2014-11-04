/*
 * IDDecoder.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDRec/API/IDDecoder.h"
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

/**
 * Default constructor using the name of the corresponding readout collection
 */
IDDecoder::IDDecoder(const std::string& collectionName) {
	LCDD& lcdd = LCDD::getInstance();
	_readout = lcdd.readout(collectionName);
	_volumeManager = lcdd.volumeManager();
	if (not _volumeManager.isValid()) {
		_volumeManager = VolumeManager(lcdd, "volman", lcdd.world(), Readout(), VolumeManager::TREE);
	}
	_tgeoMgr = Geometry::LCDD::getInstance().world().volume()->GetGeoManager();
}

/**
 * Default constructor using a readout object
 */
IDDecoder::IDDecoder(const Readout& readout) {
	LCDD& lcdd = LCDD::getInstance();
	_readout = readout;
	_volumeManager = lcdd.volumeManager();
	if (not _volumeManager.isValid()) {
		_volumeManager = VolumeManager(lcdd, "volman", lcdd.world(), Readout(), VolumeManager::TREE);
	}
	_tgeoMgr = Geometry::LCDD::getInstance().world().volume()->GetGeoManager();
}

/**
 * Destructor
 */
IDDecoder::~IDDecoder() {
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
	const TGeoMatrix& localToGlobal = this->detectorElement(volumeID).worldTransformation();
	localToGlobal.LocalToMaster(l, g);
	Position global(g[0], g[1], g[2]);
	return _readout.segmentation().cellID(local, global, volumeID);
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
	const TGeoMatrix& localToGlobal = this->detectorElement(volID).worldTransformation();
	localToGlobal.MasterToLocal(g, l);
	Position local(l[0], l[1], l[2]);
	return _readout.segmentation().cellID(local, global, volID);
}

/**
 * Returns the global position from a given cell ID
 */
Position IDDecoder::position(const CellID& cellID) const {
	double l[3];
	double g[3];
	Position local = _readout.segmentation().position(cellID);
	local.GetCoordinates(l);
	// FIXME: direct lookup of transformations seems to be broken
	//const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(cellID);
	const TGeoMatrix& localToGlobal = this->detectorElement(cellID).worldTransformation();
	localToGlobal.LocalToMaster(l, g);
	return Position(g[0], g[1], g[2]);
}

/*
 * Returns the local position from a given cell ID
 */
Position IDDecoder::localPosition(const CellID& cellID) const {
	return _readout.segmentation().position(cellID);
}

/*
 * Returns the volume ID of a given cell ID
 */
VolumeID IDDecoder::volumeID(const CellID& cellID) const {
	return _readout.segmentation()->volumeID(cellID);
}

/*
 * Returns the volume ID of a given global position
 */
VolumeID IDDecoder::volumeID(const Position& position) const {
	DetElement det = detectorElement(position);
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
	Geometry::DetElement world = Geometry::LCDD::getInstance().world();
	Geometry::DetElement det = getClosestDaughter(world, position);
	// Fixme: check if this is valid, otherwise throw exception
	return det;
}

/*
 * Calculates the neighbours of the given cell ID and adds them to the list of neighbours
 */
void IDDecoder::neighbours(const CellID& cellID, set<CellID>& neighbours) const {
	_readout.segmentation()->neighbours(cellID, neighbours);
}

/*
 * Checks if the given cell IDs are neighbours
 */
bool IDDecoder::areNeighbours(const CellID& cellID, const CellID& otherCellID) const {
	set<CellID> neighbours;
	_readout.segmentation()->neighbours(cellID, neighbours);
	return neighbours.count(otherCellID) != 0;
}

Geometry::DetElement IDDecoder::getClosestDaughter(const Geometry::DetElement& det,
		const Geometry::Position& position) {
	Geometry::DetElement result;

	// check if we have a shape and see if we are inside
	if (det.volume().isValid() and det.volume().solid().isValid()) {
		double globalPosition[3] = { position.x(), position.y(), position.z() };
		double localPosition[3] = { 0., 0., 0. };
		det.worldTransformation().MasterToLocal(globalPosition, localPosition);
		if (det.volume().solid()->Contains(localPosition)) {
			result = det;
		} else {
			// assuming that any daughter shape would be inside this shape
			return Geometry::DetElement();
		}
	}

	const DetElement::Children& children = det.children();
	DetElement::Children::const_iterator it = children.begin();
	while (it != children.end()) {
		Geometry::DetElement daughterDet = getClosestDaughter(it->second, position);
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
