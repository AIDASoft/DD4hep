/*
 * CartesianGridXYZ.h
 *
 *  Created on: Jun 28, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef DDSegmentation_CARTESIANGRIDXYZ_H_
#define DDSegmentation_CARTESIANGRIDXYZ_H_

#include "DDSegmentation/CartesianGridXY.h"

namespace dd4hep {
namespace DDSegmentation {

class CartesianGridXYZ: public CartesianGridXY {
public:
	/// default constructor using an arbitrary type
	CartesianGridXYZ(const std::string& cellEncoding);
	/// Default constructor used by derived classes passing an existing decoder
	CartesianGridXYZ(const BitFieldCoder* decoder);
	/// destructor
	virtual ~CartesianGridXYZ();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
	/// access the grid size in Z
	double gridSizeZ() const {
		return _gridSizeZ;
	}
	/// access the coordinate offset in Z
	double offsetZ() const {
		return _offsetZ;
	}
	/// access the field name used for Z
	const std::string& fieldNameZ() const {
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
	void setFieldNameZ(const std::string& fieldName) {
		_zId = fieldName;
	}
	/** \brief Returns a vector<double> of the cellDimensions of the given cell ID
	    in natural order of dimensions, e.g., dx/dy/dz, or dr/r*dPhi

	    Returns a vector of the cellDimensions of the given cell ID
	    \param cellID is ignored as all cells have the same dimension
	    \return std::vector<double> size 3:
	    -# size in x
	    -# size in y
	    -# size in z
	*/
	virtual std::vector<double> cellDimensions(const CellID& cellID) const;

protected:
	/// the grid size in Z
	double _gridSizeZ;
	/// the coordinate offset in Z
	double _offsetZ;
	/// the field name used for Z
	std::string _zId;
};

} /* namespace DDSegmentation */
} /* namespace dd4hep */
#endif /* DDSegmentation_CARTESIANGRIDXYZ_H_ */
