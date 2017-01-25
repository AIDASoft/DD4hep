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
using Geometry::Readout;
using Geometry::Solid;
using Geometry::VolumeManager;
using Geometry::Volume;
using Geometry::Position;
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
		lcdd.apply("DD4hepVolumeManager",0,0);
		_volumeManager = lcdd.volumeManager();
	}
	_tgeoMgr = lcdd.world().volume()->GetGeoManager();
}

/**
 * Returns the cell ID from the local position in the given volume ID.
 */
CellID IDDecoder::cellIDFromLocal(const Position& local, const VolumeID volID) const {
	double l[3];
	double g[3];
	local.GetCoordinates(l);
	// FIXME: direct lookup of transformations seems to be broken
	//const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(volID);
	DetElement det = this->detectorElement(volID);
	const TGeoMatrix& localToGlobal = det.nominal().worldTransformation();
	localToGlobal.LocalToMaster(l, g);
	Position global(g[0], g[1], g[2]);
	return this->findReadout(det).segmentation().cellID(local, global, volID);
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
	const TGeoMatrix& localToGlobal = det.nominal().worldTransformation();
	localToGlobal.MasterToLocal(g, l);
	Position local(l[0], l[1], l[2]);
	return this->findReadout(det).segmentation().cellID(local, global, volID);
}

/**
 * Returns the global position from a given cell ID
 */
Position IDDecoder::position(const CellID& cell) const {
	double l[3];
	double g[3];
	DetElement det = this->detectorElement(cell);
	Position local = this->findReadout(det).segmentation().position(cell);
	local.GetCoordinates(l);
	// FIXME: direct lookup of transformations seems to be broken
	//const TGeoMatrix& localToGlobal = _volumeManager.worldTransformation(cell);
	const TGeoMatrix& localToGlobal = det.nominal().worldTransformation();
	localToGlobal.LocalToMaster(l, g);
	return Position(g[0], g[1], g[2]);
}

/*
 * Returns the local position from a given cell ID
 */
Position IDDecoder::localPosition(const CellID& cell) const {
	DetElement det = this->detectorElement(cell);
	return this->findReadout(det).segmentation().position(cell);
}

/*
 * Returns the volume ID of a given cell ID
 */
VolumeID IDDecoder::volumeID(const CellID& cell) const {
	DetElement det = this->detectorElement(cell);
	return this->findReadout(det).segmentation().volumeID(cell);
}

/*
 * Returns the volume ID of a given global position
 */
VolumeID IDDecoder::volumeID(const Position& pos) const {
	DetElement det = this->detectorElement(pos);
	return det.volumeID();
}

/*
 * Returns the placement for a given cell ID
 */
PlacedVolume IDDecoder::placement(const CellID& cell) const {
	return _volumeManager.lookupPlacement(cell);
}

/*
 * Returns the placement for a given global position
 */
PlacedVolume IDDecoder::placement(const Position& pos) const {
	return placement(volumeID(pos));
}

/*
 * Returns the subdetector for a given cell ID
 */
DetElement IDDecoder::subDetector(const CellID& cell) const {
	return _volumeManager.lookupDetector(cell);
}

/*
 * Returns the subdetector for a given global position
 */
DetElement IDDecoder::subDetector(const Position& pos) const {
	return subDetector(volumeID(pos));
}

/*
 * Returns the closest detector element in the hierarchy for a given cell ID
 */
DetElement IDDecoder::detectorElement(const CellID& cell) const {
	return _volumeManager.lookupDetElement(cell);
}

/*
 * Returns the closest detector element in the hierarchy for a given global position
 */
DetElement IDDecoder::detectorElement(const Position& pos) const {
	DetElement world = Geometry::LCDD::getInstance().world();
	DetElement det = getClosestDaughter(world, pos);
	if (not det.isValid()) {
		throw invalid_position("DD4hep::DDRec::IDDecoder::detectorElement", pos);
	}
	std::cout << det.name() << std::endl;
	return det;
}

/// Access to the Readout object for a given cell ID
Geometry::Readout IDDecoder::readout(const CellID& cell) const {
	DetElement det = this->detectorElement(cell);
	return this->findReadout(det);
}

/// Access to the Readout object for a given global position
Geometry::Readout IDDecoder::readout(const Position& global) const {
	DetElement det = this->detectorElement(global);
	return this->findReadout(det);
}

/*
 * Calculates the neighbours of the given cell ID and adds them to the list of neighbours
 */
void IDDecoder::neighbours(const CellID& cell, set<CellID>& neighbour_cells) const {
	DetElement det = this->detectorElement(cell);
	this->findReadout(det).segmentation().neighbours(cell, neighbour_cells);
}

/*
 * Checks if the given cell IDs are neighbours
 */
bool IDDecoder::areNeighbours(const CellID& cell, const CellID& otherCellID) const {
	set<CellID> neighbour_cells;
	DetElement det = this->detectorElement(cell);
	this->findReadout(det).segmentation().neighbours(cell, neighbour_cells);
	return neighbour_cells.count(otherCellID) != 0;
}

/// Access to the barrel-endcap flag
IDDecoder::BarrelEndcapFlag IDDecoder::barrelEndcapFlag(const CellID& cell) const {
	Readout r = this->readout(cell);
	return BarrelEndcapFlag(r.idSpec().field(this->barrelIdentifier())->value(cell));
}

/// Access to the layer index
long int IDDecoder::layerIndex(const CellID& cell) const {
	Readout r = this->readout(cell);
	return r.idSpec().field(this->layerIdentifier())->value(cell);
}

/// Access to the system index
long int IDDecoder::systemIndex(const CellID& cell) const {
	Readout r = this->readout(cell);
	return r.idSpec().field(this->systemIdentifier())->value(cell);
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
		det.nominal().worldTransformation().MasterToLocal(globalPosition, localPosition);
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
