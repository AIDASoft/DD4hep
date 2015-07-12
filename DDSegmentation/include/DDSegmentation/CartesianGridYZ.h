#ifndef DDSegmentation_CARTESIANGRIDYZ_H_
#define DDSegmentation_CARTESIANGRIDYZ_H_

#include "DDSegmentation/CartesianGrid.h"

namespace DD4hep {
namespace DDSegmentation {

/**
 * CartesianGridYZ.h
 *
 *  @date: Sep 03, 2014
 *  @author: F.Gaede CERN/Desy
 *  @version: $Id: $
 *     direkt copy of CartesianGridXY
 *     by Christian Grefe, CERN
 */
class CartesianGridYZ: public CartesianGrid {
public:
	/// Default constructor passing the encoding string
	CartesianGridYZ(const std::string& cellEncoding = "");
	/// destructor
	virtual ~CartesianGridYZ();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
	/// access the grid size in Y
	double gridSizeY() const {
		return _gridSizeY;
	}
	/// access the grid size in Z
	double gridSizeZ() const {
		return _gridSizeZ;
	}
	/// access the coordinate offset in Y
	double offsetY() const {
		return _offsetY;
	}
	/// access the coordinate offset in Z
	double offsetZ() const {
		return _offsetZ;
	}
	/// access the field name used for Y
	const std::string& fieldNameY() const {
		return _yId;
	}
	/// access the field name used for Z
	const std::string& fieldNameZ() const {
		return _zId;
	}
	/// set the grid size in Y
	void setGridSizeY(double cellSize) {
		_gridSizeY = cellSize;
	}
	/// set the grid size in Z
	void setGridSizeZ(double cellSize) {
		_gridSizeZ = cellSize;
	}
	/// set the coordinate offset in Y
	void setOffsetY(double offset) {
		_offsetY = offset;
	}
	/// set the coordinate offset in Z
	void setOffsetZ(double offset) {
		_offsetZ = offset;
	}
	/// set the field name used for Y
	void setFieldNameY(const std::string& fieldName) {
		_yId = fieldName;
	}
	/// set the field name used for Z
	void setFieldNameZ(const std::string& fieldName) {
		_zId = fieldName;
	}
	/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
	    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

	    Returns a vector of the cellDimensions of the given cell ID
	    \param cellID is ignored as all cells have the same dimension
	    \return std::vector<double> size 2:
	    -# size in y
	    -# size in z
	*/
	virtual std::vector<double> cellDimensions(const CellID& cellID) const;

protected:
	/// the grid size in Y
	double _gridSizeY;
	/// the coordinate offset in Y
	double _offsetY;
	/// the grid size in Z
	double _gridSizeZ;
	/// the coordinate offset in Z
	double _offsetZ;
	/// the field name used for Y
	std::string _yId;
	/// the field name used for Z
	std::string _zId;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_CARTESIANGRIDXY_H_ */
