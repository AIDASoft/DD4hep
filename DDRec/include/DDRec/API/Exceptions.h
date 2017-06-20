/*
 * Exceptions.h
 *
 * Collection of Exception classes used in rec
 *
 *  Created on: Oct 31, 2014
 *      Author: Christian Grefe, Bonn University
 */

#ifndef rec_EXCEPTIONS_H_
#define rec_EXCEPTIONS_H_

#include "DD4hep/DetElement.h"
#include "DD4hep/Objects.h"

#include "DDSegmentation/Segmentation.h"

#include <stdexcept>
#include <string>
#include <sstream>

namespace dd4hep {
namespace rec {

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
	invalid_position(const std::string& msg, const Position& position) :
			std::invalid_argument(createMsg(msg, position)) {
	}
private:
	static std::string createMsg(const std::string& msg, const Position& position) {
		std::stringstream s;
		s << "Invalid position: " << msg << " " << position;
		return s.str();
	}
};

class invalid_detector_element: public std::invalid_argument {
public:
	invalid_detector_element(const std::string& msg, const DetElement& det) :
			std::invalid_argument(createMsg(msg, det)) {
	}
private:
	static std::string createMsg(const std::string& msg, const DetElement& det) {
		std::stringstream s;
		s << "Invalid detector element: " << msg;
		if (det.isValid())
			s << " (" <<det.name() << ")";
		return s.str();
	}
};

} /* namespace DD4Rec */
} /* namespace dd4hep */
#endif /* rec_EXCEPTIONS_H_ */
