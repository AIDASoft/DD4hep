/*
 * SubdetectorExtensionImpl.h
 *
 * Basic implementation of the SubdetectorExtension
 *
 *  Created on: Nov 6, 2014
 *      Author: Christian Grefe, Bonn University
 */

#ifndef rec_SUBDETECTOREXTENSIONIMPL_H_
#define rec_SUBDETECTOREXTENSIONIMPL_H_

#include "DDRec/Extensions/SubdetectorExtension.h"
#include "DD4hep/DetElement.h"

namespace dd4hep {
namespace rec {

/**
 * Class describing general parameters of a subdetector.
 * By default information is retrieved from the DetElement if possible.
 * Values can be set manually which will superseed the information from
 * the DetElement.
 */
class SubdetectorExtensionImpl: public SubdetectorExtension {
public:
	/// Default constructor using a top level DetElement
	SubdetectorExtensionImpl(const DetElement& det);

	/// Copy constructor
	SubdetectorExtensionImpl(const SubdetectorExtensionImpl& e, const DetElement& d);

	/// Destructor
	virtual ~SubdetectorExtensionImpl();

	/// Is this a barrel detector
	virtual bool isBarrel() const;

	/// Is this an endcap detector
	virtual bool isEndcap() const;

	/// Access to the inner radius
	virtual double getRMin() const;

	/// Access to the outer radius
	virtual double getRMax() const;

	/// Access to the lower z extent
	virtual double getZMin() const;

	/// Access to the upper z extent
	virtual double getZMax() const;

	/// Access to the number of sides
	/* Describes the number of corners for a polygon.
	 * Returns 0 in case of a circular shape
	 */
	virtual int getNSides() const;

	/// Sets the top level detector element used to determine shape information
	void setDetectorElement(const DetElement& det);

	/// Sets the isBarrel flag
	void setIsBarrel(bool value);

	/// Sets the isEndcap flag
	void setIsEndcap(bool value);

	/// Sets the value for the inner radius
	void setRMin(double value);

	/// Sets the value for the outer radius
	void setRMax(double value);

	/// Sets the value for the inner z extent
	void setZMin(double value);

	/// Sets the value for the outer z extent
	void setZMax(double value);

	/// Sets the number of sides
	void setNSides(int value);

protected:
	DetElement det;
	bool _isBarrel;
	bool _setIsBarrel;
	bool _isEndcap;
	bool _setIsEndcap;
	double _rMin;
	bool _setRMin;
	double _rMax;
	bool _setRMax;
	double _zMin;
	bool _setZMin;
	double _zMax;
	bool _setZMax;
	int _nSides;
	bool _setNSides;

	void resetAll();
};

} /* namespace rec */
} /* namespace dd4hep */
#endif /* rec_SUBDETECTOREXTENSIONIMPL_H_ */
