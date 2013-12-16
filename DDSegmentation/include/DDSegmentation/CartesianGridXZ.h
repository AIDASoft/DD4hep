/*
 * CartesianGridXY.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CARTESIANGRIDXZ_H_
#define DDSegmentation_CARTESIANGRIDXZ_H_

#include "DDSegmentation/CartesianGrid.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianGridXZ: public CartesianGrid {
public:
	/// default constructor using an arbitrary type
	CartesianGridXZ(const std::string& cellEncoding);
	/// destructor
	virtual ~CartesianGridXZ();

	/// determine the local based on the cell ID
	virtual Position position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Position& localPosition, const Position& globalPosition, const VolumeID& volumeID) const;
	/// access the grid size in X
	double gridSizeX() const {
		return _gridSizeX;
	}
	/// access the grid size in Z
	double gridSizeZ() const {
		return _gridSizeZ;
	}
	/// access the coordinate offset in X
	double offsetX() const {
		return _offsetX;
	}
	/// access the coordinate offset in Z
	double offsetZ() const {
		return _offsetZ;
	}
	/// access the field name used for X
	const std::string& fieldNameX() const {
		return _xId;
	}
	/// access the field name used for Z
	const std::string& fieldNameZ() const {
		return _zId;
	}
	/// set the grid size in X
	void setGridSizeX(double cellSize) {
		_gridSizeX = cellSize;
	}
	/// set the grid size in Z
	void setGridSizeZ(double cellSize) {
		_gridSizeZ = cellSize;
	}
	/// set the coordinate offset in X
	void setOffsetX(double offset) {
		_offsetX = offset;
	}
	/// set the coordinate offset in Z
	void setOffsetZ(double offset) {
		_offsetZ = offset;
	}
	/// set the field name used for X
	void setFieldNameX(const std::string& name) {
		_xId = name;
	}
	/// set the field name used for Y
	void setFieldNameZ(const std::string& name) {
		_zId = name;
	}

protected:
	/// the grid size in X
	double _gridSizeX;
	/// the coordinate offset in X
	double _offsetX;
	/// the grid size in Z
	double _gridSizeZ;
	/// the coordinate offset in Z
	double _offsetZ;
	/// the field name used for X
	std::string _xId;
	/// the field name used for Z
	std::string _zId;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRIDXZ_H_ */
