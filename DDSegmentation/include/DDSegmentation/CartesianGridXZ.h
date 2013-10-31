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
	template <typename TYPE> CartesianGridXZ(TYPE cellEncoding, double gridSizeX = 1., double gridSizeZ = 1., double offsetX = 0.,
			double offsetZ = 0., const std::string& xField = "x", const std::string& zField = "z");
	/// destructor
	virtual ~CartesianGridXZ();

	/// determine the local based on the cell ID
	virtual std::vector<double> getPosition(const long64& cellID) const;
	/// determine the cell ID based on the position
	virtual long64 getCellID(double x, double y, double z) const;
	/// access the grid size in X
	double getGridSizeX() const {
		return _gridSizeX;
	}
	/// access the grid size in Z
	double getGridSizeZ() const {
		return _gridSizeZ;
	}
	/// access the coordinate offset in X
	double getOffsetX() const {
		return _offsetX;
	}
	/// access the coordinate offset in Z
	double getOffsetZ() const {
		return _offsetZ;
	}
	/// access the field name used for X
	std::string getFieldNameX() const {
		return _xId;
	}
	/// access the field name used for Z
	std::string getFieldNameZ() const {
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
	/// access the set of parameters for this segmentation
	Parameters parameters() const;

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
