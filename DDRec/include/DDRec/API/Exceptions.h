/*
 * Exceptions.h
 *
 * Collection of Exception classes used in DDRec
 *
 *  Created on: Oct 31, 2014
 *      Author: Christian Grefe, Bonn University
 */

#ifndef DDRec_EXCEPTIONS_H_
#define DDRec_EXCEPTIONS_H_

#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"

#include "DDSegmentation/Segmentation.h"

#include <stdexcept>
#include <string>
#include <strstream>

namespace DD4hep {
namespace DDRec {

class invalid_cell_id: public std::invalid_argument {
public:
	invalid_cell_id(const std::string& msg, const DDSegmentation::CellID& cellID = 0) :
			std::invalid_argument(createMsg(msg, cellID)) {
	}
private:
	static std::string createMsg(const std::string& msg, const DDSegmentation::CellID& cellID) {
		std::stringstream s;
		s << "Invalid cell ID: " << msg;
		if (cellID)
			s << " (" << cellID << ")";
		return s.str();
	}
};

class invalid_position: public std::invalid_argument {
public:
	invalid_position(const std::string& msg, const Geometry::Position& position) :
			std::invalid_argument(createMsg(msg, position)) {
	}
private:
	static std::string createMsg(const std::string& msg, const Geometry::Position& position) {
		std::stringstream s;
		s << "Invalid position: " << msg << " " << position;
		return s.str();
	}
};

class invalid_detector_element: public std::invalid_argument {
public:
	invalid_detector_element(const std::string& msg, const Geometry::DetElement& det) :
			std::invalid_argument(createMsg(msg, det)) {
	}
private:
	static std::string createMsg(const std::string& msg, const Geometry::DetElement& det) {
		std::stringstream s;
		s << "Invalid detector element: " << msg;
		if (det.isValid())
			s << " (" <<det.name() << ")";
		return s.str();
	}
};

} /* namespace DD4Rec */
} /* namespace DD4hep */
#endif /* DDRec_EXCEPTIONS_H_ */
