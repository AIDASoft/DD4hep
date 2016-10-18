/*
 * ProjectiveCylinder.h
 *
 *  Created on: Oct 31, 2013
 *      Author: Christian Grefe, CERN
 */

#ifndef PROJECTIVECYLINDER_H_
#define PROJECTIVECYLINDER_H_

#include "CylindricalSegmentation.h"

namespace DD4hep {
namespace DDSegmentation {

class ProjectiveCylinder: public CylindricalSegmentation {
public:
	/// default constructor using an arbitrary type
	ProjectiveCylinder(const std::string& cellEncoding);
	/// Default constructor used by derived classes passing an existing decoder
	ProjectiveCylinder(BitField64* decoder);
	/// destructor
	virtual ~ProjectiveCylinder();

	/// determine the position based on the cell ID
	virtual Vector3D position(const CellID& cellID) const;
	/// determine the cell ID based on the position
	virtual CellID cellID(const Vector3D& localPosition, const Vector3D& globalPosition, const VolumeID& volumeID) const;
	/// determine the polar angle theta based on the cell ID
	double theta(const CellID& cellID) const;
	/// determine the azimuthal angle phi based on the cell ID
	double phi(const CellID& cellID) const;
	/// access the number of bins in theta
	int thetaBins() const {
		return _thetaBins;
	}
	/// access the number of bins in theta
	int phiBins() const {
		return _phiBins;
	}
	/// access the coordinate offset in theta
	double offsetTheta() const {
		return _offsetTheta;
	}
	/// access the coordinate offset in phi
	double offsetPhi() const {
		return _offsetPhi;
	}
	/// access the field name used for theta
	std::string fieldNameTheta() const {
		return _thetaID;
	}
	/// access the field name used for phi
	std::string fieldNamePhi() const {
		return _phiID;
	}
	/// set the number of bins in theta
	void setThetaBins(int bins) {
		_thetaBins = bins;
	}
	/// set the number of bins in phi
	void setPhiBins(int bins) {
		_phiBins = bins;
	}
	/// set the coordinate offset in theta
	void setOffsetTheta(double offset) {
		_offsetTheta = offset;
	}
	/// set the coordinate offset in phi
	void setOffsetPhi(double offset) {
		_offsetPhi = offset;
	}
	/// set the field name used for theta
	void setFieldNameTheta(const std::string& fieldName) {
		_thetaID = fieldName;
	}
	/// set the field name used for phi
	void setFieldNamePhi(const std::string& fieldName) {
		_phiID = fieldName;
	}

protected:
	/// the number of bins in theta
	int _thetaBins;
	/// the number of bins in phi
	int _phiBins;
	/// the coordinate offset in theta
	double _offsetTheta;
	/// the coordinate offset in phi
	double _offsetPhi;
	/// the field name used for theta
	std::string _thetaID;
	/// the field name used for phi
	std::string _phiID;

	/// determine the polar angle theta based on the current cell ID
	double theta() const;
	/// determine the azimuthal angle phi based on the current cell ID
	double phi() const;

};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* PROJECTIVECYLINDER_H_ */
