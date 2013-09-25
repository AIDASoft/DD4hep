/*
 * CartesianGridXYZ.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CARTESIANGRIDXYZ_H_
#define DDSegmentation_CARTESIANGRIDXYZ_H_

#include "DDSegmentation/CartesianGridXY.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianGridXYZ: public CartesianGridXY {
public:
	/// default constructor using an arbitrary type
	template <typename TYPE> CartesianGridXYZ(TYPE cellEncoding, double gridSizeX = 1., double gridSizeY = 1., double gridSizeZ = 1.,
			double offsetX = 0., double offsetY = 0., double offsetZ = 0., std::string xField = "x",
			std::string yField = "y", std::string zField = "z");
	/// default constructor using an existing decoder
	CartesianGridXYZ(BitField64* decoder, double gridSizeX = 1., double gridSizeY = 1., double gridSizeZ = 1., double offsetX =
			0., double offsetY = 0., double offsetZ = 0., std::string xField = "x", std::string yField = "y",
			std::string zField = "z");
	/// destructor
	virtual ~CartesianGridXYZ();

	/// determine the local position based on the cell ID
	virtual std::vector<double> getLocalPosition(const long64& cellID) const;
	/// determine the cell ID based on the local position
	virtual long64 getCellID(double x, double y, double z) const;
	/// access the grid size in Z
	double getGridSizeZ() const {
		return _gridSizeZ;
	}
	/// access the coordinate offset in Z
	double getOffsetZ() const {
		return _offsetZ;
	}
	/// access the field name used for Z
	std::string getFieldNameZ() const {
		return _zId;
	}
	/// set the grid size in Z
	void setGridSizeZ(double cellSize) {
		_gridSizeZ = cellSize;
	}
	/// set the coordinate offset in Z
	void setOffsetZ(double offset) {
		_offsetZ = offset;
	}
	/// set the field name used for Z
	void setFieldNameZ(std::string name) {
		_zId = name;
	}
	/// access the set of parameters for this segmentation
	Parameters parameters() const;

protected:
	/// the grid size in Z
	double _gridSizeZ;
	/// the coordinate offset in Z
	double _offsetZ;
	/// the field name used for Z
	std::string _zId;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRIDXYZ_H_ */
