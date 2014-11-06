/*
 * IDDecoder.h
 *
 *  Created on: Dec 12, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef IDDECODER_H_
#define IDDECODER_H_

#include "DD4hep/Readout.h"
#include "DD4hep/VolumeManager.h"

#include "DDSegmentation/Segmentation.h"

#include <set>
#include <string>

class TGeoManager;

namespace DD4hep {
namespace DDRec {

typedef DDSegmentation::CellID CellID;
typedef DDSegmentation::VolumeID VolumeID;

/**
 * Combines functionality of the VolumeManager and Segmentation classes to provide a
 * high level interface for position to cell ID and cell ID to position conversions
 * and related information.
 */
class IDDecoder {
public:
	/**
	 * Default constructor using the name of the corresponding readout collection
	 */
	IDDecoder(const std::string& collectionName);

	/**
	 * Default constructor using a readout object
	 */
	IDDecoder(const Geometry::Readout& readout);

	/**
	 * Destructor
	 */
	virtual ~IDDecoder();

	/**
	 * Returns the cell ID from the local position in the given volume ID.
	 */
	CellID cellIDFromLocal(const Geometry::Position& local, const VolumeID volumeID) const;

	/**
	 * Returns the global cell ID from a given global position
	 */
	CellID cellID(const Geometry::Position& global) const;

	/**
	 * Returns the global position from a given cell ID
	 */
	Geometry::Position position(const CellID& cellID) const;

	/*
	 * Returns the local position from a given cell ID
	 */
	Geometry::Position localPosition(const CellID& cellID) const;

	/*
	 * Returns the volume ID of a given cell ID
	 */
	VolumeID volumeID(const CellID& cellID) const;

	/*
	 * Returns the volume ID of a given global position
	 */
	VolumeID volumeID(const Geometry::Position& global) const;

	/*
	 * Returns the placement for a given cell ID
	 */
	Geometry::PlacedVolume placement(const CellID& cellID) const;

	/*
	 * Returns the placement for a given global position
	 */
	Geometry::PlacedVolume placement(const Geometry::Position& global) const;

	/*
	 * Returns the subdetector for a given cell ID
	 */
	Geometry::DetElement subDetector(const CellID& cellID) const;

	/*
	 * Returns the subdetector for a given global position
	 */
	Geometry::DetElement subDetector(const Geometry::Position& global) const;

	/*
	 * Returns the closest detector element in the hierarchy for a given cell ID
	 */
	Geometry::DetElement detectorElement(const CellID& cellID) const;

	/*
	 * Returns the closest detector element in the hierarchy for a given global position
	 */
	Geometry::DetElement detectorElement(const Geometry::Position& global) const;

	/*
	 * Calculates the neighbours of the given cell ID and adds them to the list of neighbours
	 */
	void neighbours(const CellID& cellID, std::set<CellID>& neighbours) const;

	/*
	 * Checks if the given cell IDs are neighbours
	 */
	bool areNeighbours(const CellID& cellID, const CellID& otherCellID) const;

protected:
	Geometry::Readout _readout;
	Geometry::VolumeManager _volumeManager;
	TGeoManager* _tgeoMgr;

	// helper method to get the closest daughter DetElement to the position starting from the given DetElement
	static Geometry::DetElement getClosestDaughter(const Geometry::DetElement& det, const Geometry::Position& position);
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* IDDECODER_H_ */
