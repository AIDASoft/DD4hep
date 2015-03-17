/*
 * PolarGridRPhi2.h
 *
 *  Created on: Sept 13, 2014
 *      Author: Marko Petric
 */

#ifndef DDSegmentation_PolarGridRPhi2_H_
#define DDSegmentation_PolarGridRPhi2_H_

#include "DDSegmentation/PolarGrid.h"
#include <math.h>
#include <vector>

namespace DD4hep {
namespace DDSegmentation {

class PolarGridRPhi2: public PolarGrid {
public:
	/// Default constructor passing the encoding string
	PolarGridRPhi2(const std::string& cellEncoding = "");
	/// destructor
	virtual ~PolarGridRPhi2();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
	/// access the grid size in R
	std::vector<double> gridRValues() const {
		return _gridRValues;
	}
	/// access the grid size in Phi
	double gridSizePhi() const {
		return _gridSizePhi;
	}
	/// access the coordinate offset in R
	double offsetR() const {
		return _offsetR;
	}
	/// access the coordinate offset in Phi
	double offsetPhi() const {
		return _offsetPhi;
	}
	/// access the field name used for R
	const std::string& fieldNameR() const {
		return _rId;
	}
	/// access the field name used for Phi
	const std::string& fieldNamePhi() const {
		return _phiId;
	}
	/// set the grid size in R
	void setgridRValues(double cellSize, int value) {
		_gridRValues[value] = cellSize;
	}
	/// set the grid size in Phi
	void setGridSizePhi(double cellSize) {
		_gridSizePhi = cellSize;
	}
	/// set the coordinate offset in R
	void setOffsetR(double offset) {
		_offsetR = offset;
	}
	/// set the coordinate offset in Phi
	void setOffsetPhi(double offset) {
		_offsetPhi = offset;
	}
	/// set the field name used for X
	void setFieldNameR(const std::string& name) {
		_rId = name;
	}
	/// set the field name used for Y
	void setFieldNamePhi(const std::string& name) {
		_phiId = name;
	}

protected:
	/// the grid size in X
	std::vector<double> _gridRValues;
	/// the coordinate offset in X
	double _offsetR;
	/// the grid size in Y
	double _gridSizePhi;
	/// the coordinate offset in Y
	double _offsetPhi;
	/// the field name used for X
	std::string _rId;
	/// the field name used for Y
	std::string _phiId;
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* DDSegmentation_PolarGridRPhi2_H_ */
