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
	class BarrelEndcapFlag {
	public:
		enum BarrelEncapID {
			Barrel=0,
			EndcapSouth,
			EndcapNorth,
			n_BarrelEndcapID
		};
		BarrelEndcapFlag(unsigned int val) :
			value(static_cast<BarrelEncapID>(val)) {}

		virtual ~BarrelEndcapFlag() {}

		bool isBarrel() const {
			return value == Barrel;
		}

		bool isEndcap() const {
			return value == EndcapNorth || value == EndcapSouth;
		}

		bool isEndcapSouth() const {
			return value == EndcapSouth;
		}

		bool isEndcapNorth() const {
			return value == EndcapSouth;
		}

		BarrelEncapID getValue() const {
			return value;
		}

	protected:
		BarrelEncapID value;
	};

	/// Access to the global IDDecoder instance
	static IDDecoder& getInstance();

	/// Destructor
	virtual ~IDDecoder() {};

	/// Returns the cell ID from the local position in the given volume ID.
	CellID cellIDFromLocal(const Position& local, const VolumeID volumeID) const;

	/// Returns the global cell ID from a given global position
	CellID cellID(const Position& global) const;

	/// Returns the global position from a given cell ID
	Position position(const CellID& cellID) const;

	/// Returns the local position from a given cell ID
	Position localPosition(const CellID& cellID) const;

	/// Returns the volume ID of a given cell ID
	VolumeID volumeID(const CellID& cellID) const;

	/// Returns the volume ID of a given global position
	VolumeID volumeID(const Position& global) const;

	/// Returns the placement for a given cell ID
	Geometry::PlacedVolume placement(const CellID& cellID) const;

	/// Returns the placement for a given global position
	Geometry::PlacedVolume placement(const Position& global) const;

	/// Returns the subdetector for a given cell ID
	Geometry::DetElement subDetector(const CellID& cellID) const;

	/// Returns the subdetector for a given global position
	Geometry::DetElement subDetector(const Position& global) const;

	/// Returns the closest detector element in the hierarchy for a given cell ID
	Geometry::DetElement detectorElement(const CellID& cellID) const;

	/// Returns the closest detector element in the hierarchy for a given global position
	Geometry::DetElement detectorElement(const Position& global) const;

	/// Access to the Readout object for a given cell ID
	Geometry::Readout readout(const CellID& cellID) const;

	/// Access to the Readout object for a given global position
	Geometry::Readout readout(const Position& global) const;

	/// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
	void neighbours(const CellID& cellID, std::set<CellID>& neighbours) const;

	/// Checks if the given cell IDs are neighbours
	bool areNeighbours(const CellID& cellID, const CellID& otherCellID) const;

	/// Access to the barrel-endcap flag
	BarrelEndcapFlag barrelEndcapFlag(const CellID& cellID) const;

	/// Access to the layer index
	long int layerIndex(const CellID& cellID) const;

	/// Access to the system index
	long int systemIndex(const CellID& cellID) const;

	static std::string barrelIdentifier() {
		return std::string("barrel");
	}

	static std::string layerIdentifier() {
		return std::string("layer");
	}

	static std::string systemIdentifier() {
		return std::string("system");
	}

protected:
	Geometry::VolumeManager _volumeManager;
	TGeoManager* _tgeoMgr;

	/// Helper method to find the corresponding Readout object to a DetElement
	Geometry::Readout findReadout(const Geometry::DetElement& det) const;

	/// Helper method to get the closest daughter DetElement to the position starting from the given DetElement
	static Geometry::DetElement getClosestDaughter(const Geometry::DetElement& det, const Position& position);

private:
	/// Default constructor
	IDDecoder();

	/// Disable copy constructor
	IDDecoder(const IDDecoder&);

	/// Disable assignment operator
	void operator=(const IDDecoder&);
};

} /* namespace DDRec */
} /* namespace DD4hep */
#endif /* IDDECODER_H_ */
