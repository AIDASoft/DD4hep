/*
 * CartesianGridXY.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CARTESIANGRIDXY_H_
#define DDSegmentation_CARTESIANGRIDXY_H_

#include "DDSegmentation/CartesianGrid.h"

namespace DD4hep {
namespace DDSegmentation {

class CartesianGridXY: public CartesianGrid {
public:
	/// default constructor using an arbitrary type
	template <typename TYPE> CartesianGridXY(TYPE cellEncoding, double gridSizeX = 1., double gridSizeY = 1., double offsetX = 0.,
			double offsetY = 0., std::string xField = "x", std::string yField = "y");
	/// default constructor using an existing decoder
	CartesianGridXY(BitField64* decoder, double gridSizeX = 1., double gridSizeY = 1., double offsetX = 0.,
			double offsetY = 0., std::string xField = "x", std::string yField = "y");
	/// destructor
	virtual ~CartesianGridXY();

	/// determine the local position based on the cell ID
	virtual std::vector<double> getLocalPosition(const long64& cellID) const;
	/// determine the cell ID based on the local position
	virtual long64 getCellID(double x, double y, double z) const;
	/// access the grid size in X
	double getGridSizeX() const {
		return _gridSizeX;
	}
	/// access the grid size in Y
	double getGridSizeY() const {
		return _gridSizeY;
	}
	/// access the coordinate offset in X
	double getOffsetX() const {
		return _offsetX;
	}
	/// access the coordinate offset in Y
	double getOffsetY() const {
		return _offsetY;
	}
	/// access the field name used for X
	std::string getFieldNameX() const {
		return _xId;
	}
	/// access the field name used for Y
	std::string getFieldNameY() const {
		return _yId;
	}
	/// set the grid size in X
	void setGridSizeX(double cellSize) {
		_gridSizeX = cellSize;
	}
	/// set the grid size in Y
	void setGridSizeY(double cellSize) {
		_gridSizeY = cellSize;
	}
	/// set the coordinate offset in X
	void setOffsetX(double offset) {
		_offsetX = offset;
	}
	/// set the coordinate offset in Y
	void setOffsetY(double offset) {
		_offsetY = offset;
	}
	/// set the field name used for X
	void setFieldNameX(std::string name) {
		_xId = name;
	}
	/// set the field name used for Y
	void setFieldNameY(std::string name) {
		_yId = name;
	}
	/// access the set of parameters for this segmentation
	Parameters parameters() const;

protected:
	/// the grid size in X
	double _gridSizeX;
	/// the coordinate offset in X
	double _offsetX;
	/// the grid size in Y
	double _gridSizeY;
	/// the coordinate offset in Y
	double _offsetY;
	/// the field name used for X
	std::string _xId;
	/// the field name used for Y
	std::string _yId;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRIDXY_H_ */
