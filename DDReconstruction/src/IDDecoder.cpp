/*
 * IDDecoder.cpp
 *
 *  Created on: Dec 12, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDReconstruction/API/IDDecoder.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/VolumeManager.h"

namespace DD4hep {

using Geometry::DetElement;
using Geometry::LCDD;
using Geometry::PlacedVolume;
using Geometry::Position;
using Geometry::Readout;
using Geometry::VolumeManager;
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
	const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(volumeID);
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
	const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(volID);
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
	std::cout << "Local: " << local << std::endl;
	local.GetCoordinates(l);
	const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(cellID);
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
	// FIXME: Need to navigate volumes to find enclosing volume and extract its volume ID
	return 0;
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
	return detectorElement(volumeID(position));
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

} /* namespace DD4hep */
