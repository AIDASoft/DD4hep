/*
 * SubdetectorExtensionImpl.cpp
 *
 *  Created on: Nov 6, 2014
 *      Author: Christian Grefe, Bonn University
 */

#include "DDRec/Extensions/SubdetectorExtensionImpl.h"
#include "DDRec/API/IDDecoder.h"

#include "DD4hep/LCDD.h"

namespace DD4hep {
namespace DDRec {

using Geometry::DetElement;

/// Constructor using a top level DetElement
SubdetectorExtensionImpl::SubdetectorExtensionImpl(const Geometry::DetElement& det_elt) {
	this->resetAll();
	this->det = det_elt;
}

/// Copy constructor
SubdetectorExtensionImpl::SubdetectorExtensionImpl(const SubdetectorExtensionImpl& e, const Geometry::DetElement& d) {
	this->det = d;
	if (e._setIsBarrel) {
		this->setIsBarrel(e.isBarrel());
	}
	if (e._setIsEndcap) {
		this->setIsEndcap(e.isEndcap());
	}
	if (e._setRMin) {
		this->setRMin(e.getRMin());
	}
	if (e._setRMax) {
		this->setRMax(e.getRMax());
	}
	if (e._setZMin) {
		this->setZMin(e.getZMin());
	}
	if (e._setZMax) {
		this->setZMax(e.getZMax());
	}
	if (e._setNSides) {
		this->setNSides(e.getNSides());
	}
}

/// Destructor
SubdetectorExtensionImpl::~SubdetectorExtensionImpl() {
	// nothing to do
}

/// Is this a barrel detector
bool SubdetectorExtensionImpl::isBarrel() const {
	if (_setIsBarrel) {
		return _isBarrel;
	}
	IDDecoder& id = IDDecoder::getInstance();
	return id.barrelEndcapFlag(det.volumeID()).isBarrel();
}

/// Is this an endcap detector
bool SubdetectorExtensionImpl::isEndcap() const {
	if (_setIsEndcap) {
		return _isEndcap;
	}
	IDDecoder& id = IDDecoder::getInstance();
	return id.barrelEndcapFlag(det.volumeID()).isEndcap();
}

/// Access to the inner radius
double SubdetectorExtensionImpl::getRMin() const {
	if (_setRMin) {
		return _rMin;
	}
	if (det.isValid() and det.volume().isValid() and det.volume().solid().isValid()) {
		Geometry::Solid solid = det.volume().solid();
		try {
			Geometry::Tube tube(solid);
			if (tube.isValid()) {
				return tube->GetRmin();
			}
		} catch (std::runtime_error& e) {
		}
		try {
			Geometry::PolyhedraRegular polyhedra(solid);
			if (polyhedra.isValid()) {
				return polyhedra->GetRmin()[0];
			}
		} catch (std::runtime_error& e) {
		}
	}
	return 0.;
}

/// Access to the outer radius
double SubdetectorExtensionImpl::getRMax() const {
	if (_setRMax) {
		return _rMax;
	}
	if (det.isValid() and det.volume().isValid() and det.volume().solid().isValid()) {
		Geometry::Solid solid = det.volume().solid();
		try {
			Geometry::Tube tube(solid);
			if (tube.isValid()) {
				return tube->GetRmin();
			}
		} catch (std::runtime_error& e) {
		}
		try {
			Geometry::PolyhedraRegular polyhedra(solid);
			if (polyhedra.isValid()) {
				return polyhedra->GetRmax()[0];
			}
		} catch (std::runtime_error& e) {
		}
	}
	return 0.;
}

/// Access to the lower z extent
double SubdetectorExtensionImpl::getZMin() const {
	if (_setZMin) {
		return _zMin;
	}
	if (det.isValid() and det.volume().isValid() and det.volume().solid().isValid()) {
		Geometry::Solid solid = det.volume().solid();
		Geometry::Box box(solid);
		if (box.isValid()) {
			Position local(0.,0.,-box->GetDZ()/2.);
			Position global;
			det.localToWorld(local, global);
			return global.z();
		}
	}
	return 0.;
}

/// Access to the upper z extent
double SubdetectorExtensionImpl::getZMax() const {
	if (_setZMax) {
		return _zMax;
	}
	if (det.isValid() and det.volume().isValid() and det.volume().solid().isValid()) {
		Geometry::Solid solid = det.volume().solid();
		Geometry::Box box(solid);
		if (box.isValid()) {
			Position local(0.,0.,box->GetDZ()/2.);
			Position global;
			det.localToWorld(local, global);
			return global.z();
		}
	}
	return 0.;
}

/// Access to the number of sides
/* Describes the number of corners for a polygon.
 * Returns 0 in case of a circular shape
 */
int SubdetectorExtensionImpl::getNSides() const {
	if (_setNSides) {
		return _nSides;
	}
	if (det.isValid() and det.volume().isValid() and det.volume().solid().isValid()) {
		Geometry::Solid solid = det.volume().solid();
		try {
			Geometry::PolyhedraRegular polyhedra(solid);
			if (polyhedra.isValid()) {
				return polyhedra->GetNedges();
			}
		} catch (std::runtime_error& e) {

		}
	}
	return 0;
}

/// Sets the top level detector element used to determine shape information
void SubdetectorExtensionImpl::setDetectorElement(const Geometry::DetElement& det_elt) {
	this->det = det_elt;
}

/// Sets the isBarrel flag
void SubdetectorExtensionImpl::setIsBarrel(bool value) {
	_isBarrel = value;
}

/// Sets the isEndcap flag
void SubdetectorExtensionImpl::setIsEndcap(bool value) {
	_isEndcap = value;
}

/// Sets the value for the inner radius
void SubdetectorExtensionImpl::setRMin(double value) {
	_setRMin = true;
	_rMin = value;
}

/// Sets the value for the outer radius
void SubdetectorExtensionImpl::setRMax(double value) {
	_setRMax = true;
	_rMax = value;
}

/// Sets the value for the inner z extent
void SubdetectorExtensionImpl::setZMin(double value) {
	_setZMin = true;
	_zMin = value;
}

/// Sets the value for the outer z extent
void SubdetectorExtensionImpl::setZMax(double value) {
	_setZMax = true;
	_zMax = value;
}

/// Sets the number of sides
void SubdetectorExtensionImpl::setNSides(int value) {
	_setNSides = true;
	_nSides = value;
}

void SubdetectorExtensionImpl::resetAll() {
	_isBarrel = false;
	_setIsBarrel = false;
	_isEndcap = false;
	_setIsEndcap = false;
	_rMin = 0.;
	_setRMin = false;
	_rMax = 0.;
	_setRMax = false;
	_zMin = 0.;
	_setZMin = false;
	_zMax = 0.;
	_setZMax = false;
	_nSides = 0;
	_setNSides = false;
}

} /* namespace DDRec */
} /* namespace DD4hep */
