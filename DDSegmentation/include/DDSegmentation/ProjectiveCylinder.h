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
	template<typename TYPE> ProjectiveCylinder(TYPE cellEncoding, int thetaBins = 1, int phiBins = 1,
			double offsetTheta = 0., double offsetPhi = 0., const std::string& thetaField = "theta",
			const std::string& phiField = "phi", const std::string& layerField = "layer");
	/// destructor
	virtual ~ProjectiveCylinder();

	/// determine the position based on the cell ID
	virtual std::vector<double> getPosition(const long64& cellID) const;
	/// determine the cell ID based on the position
	virtual long64 getCellID(double x, double y, double z) const;
	/// determine the polar angle theta based on the cell ID
	double getTheta(const long64& cellID) const;
	/// determine the azimuthal angle phi based on the cell ID
	double getPhi(const long64& cellID) const;
	/// access the number of bins in theta
	int getThetaBins() const {
		return _thetaBins;
	}
	/// access the number of bins in theta
	int getPhiBins() const {
		return _phiBins;
	}
	/// access the coordinate offset in theta
	double getOffsetTheta() const {
		return _offsetTheta;
	}
	/// access the coordinate offset in phi
	double getOffsetPhi() const {
		return _offsetPhi;
	}
	/// access the field name used for theta
	std::string getFieldNameTheta() const {
		return _thetaID;
	}
	/// access the field name used for phi
	std::string getFieldNamePhi() const {
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
	void setFieldNameTheta(const std::string& name) {
		_thetaID = name;
	}
	/// set the field name used for phi
	void setFieldNamePhi(const std::string& name) {
		_phiID = name;
	}
	/// access the set of parameters for this segmentation
	Parameters parameters() const;

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
};

} /* namespace DDSegmentation */
} /* namespace DD4hep */
#endif /* PROJECTIVECYLINDER_H_ */
