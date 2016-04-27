/*
 * WaferGridXY.h
 *
 *  Created on: April 19, 2016
 *      Author: S. Lu, DESY
 */

#ifndef DDSegmentation_WAFERGRIDXY_H_
#define DDSegmentation_WAFERGRIDXY_H_

#include "DDSegmentation/CartesianGrid.h"

#define MAX_GROUPS 100
#define MAX_WAFERS 100

namespace DD4hep {
namespace DDSegmentation {

class WaferGridXY: public CartesianGrid {
public:
	/// Default constructor passing the encoding string
	WaferGridXY(const std::string& cellEncoding = "");
	/// destructor
	virtual ~WaferGridXY();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
	/// access the grid size in X
	double gridSizeX() const {
		return _gridSizeX;
	}
	/// access the grid size in Y
	double gridSizeY() const {
		return _gridSizeY;
	}
	/// access the coordinate offset in X
	double offsetX() const {
		return _offsetX;
	}
	/// access the coordinate offset in Y
	double offsetY() const {
		return _offsetY;
	}
        /// access the coordinate waferOffset for inGroup in X
	double waferOffsetX(int inGroup, int inWafer) const {
	  return  _waferOffsetX[inGroup][inWafer];
	}
        /// access the coordinate waferOffset for inGroup in Y
	double waferOffsetY(int inGroup, int inWafer) const {
	  return  _waferOffsetY[inGroup][inWafer];
	}

	/// access the field name used for X
	const std::string& fieldNameX() const {
		return _xId;
	}
	/// access the field name used for Y
	const std::string& fieldNameY() const {
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
        /// set the coordinate waferOffset for inlayer in X
        void setWaferOffsetX(int inGroup,int inWafer, double offset) {
          _waferOffsetX[inGroup][inWafer] = offset;
        }
        /// set the coordinate waferOffset for inGroup in Y
        void setWaferOffsetY(int inGroup,int inWafer, double offset) {
          _waferOffsetY[inGroup][inWafer] = offset;
        }

	/// set the field name used for X
	void setFieldNameX(const std::string& fieldName) {
		_xId = fieldName;
	}
	/// set the field name used for Y
	void setFieldNameY(const std::string& fieldName) {
		_yId = fieldName;
	}
	/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
	    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

	    Returns a vector of the cellDimensions of the given cell ID
	    \param cellID is ignored as all cells have the same dimension
	    \return std::vector<double> size 2:
	    -# size in x
	    -# size in y
	*/
	virtual std::vector<double> cellDimensions(const CellID& cellID) const;

protected:
	/// the grid size in X
	double _gridSizeX;
	/// the coordinate offset in X
	double _offsetX;
	/// the grid size in Y
	double _gridSizeY;
	/// the coordinate offset in Y
	double _offsetY;
        /// list of wafer x offset for each group
	double _waferOffsetX[MAX_GROUPS][MAX_WAFERS];
        /// list of wafer y offset for each group
	double _waferOffsetY[MAX_GROUPS][MAX_WAFERS];
	/// the field name used for X
	std::string _xId;
	/// the field name used for Y
	std::string _yId;
        /// encoding field used for the Magic Wafer group
        std::string _identifierMGWaferGroup; 
        /// encoding field used for the wafer
        std::string _identifierWafer; 
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_WAFERGRIDXY_H_ */
