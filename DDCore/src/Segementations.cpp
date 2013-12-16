// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/Segmentations.h"
#include "DD4hep/InstanceCount.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Standard constructor
Segmentation::Object::Object(BaseSegmentation* s)
  : magic(magic_word()), useForHitPosition(0), segmentation(s) {
  InstanceCount::increment(this);
}

/// Default destructor
Segmentation::Object::~Object() {
  InstanceCount::decrement(this);
}

/// determine the local position based on the cell ID
DDSegmentation::Position Segmentation::Object::position(const long64& cellID) const {
	return segmentation->position(cellID);
}

/// determine the cell ID based on the local position
long64 Segmentation::Object::cellID(const DDSegmentation::Position& localPosition, const DDSegmentation::Position& globalPosition, const long64& volumeID) const {
	return segmentation->cellID(localPosition,globalPosition, volumeID);
}

/// Accessor: Segmentation type
std::string Segmentation::type() const {
  return segmentation()->type();
}

bool Segmentation::useForHitPosition() const {
  return object<Object>().useForHitPosition != 0;
}

/// Access to the parameters
DDSegmentation::Parameters Segmentation::parameters() const {
  Object& o = object<Object>();
  if (o.segmentation != 0)  {
    return o.segmentation->parameters();
  }
  throw runtime_error("DD4hep: The segmentation object " + string(type()) + " is invalid!");
}

/// Access segmentation object
DD4hep::DDSegmentation::Segmentation* Segmentation::segmentation() const {
  Object& o = object<Object>();
  if (o.segmentation != 0)
    return o.segmentation;
  throw runtime_error(
      "DD4hep: The segmentation object " + string(type())
          + " knows no implementation object [This is no longer allowed in the presence of DDSegmentation]");
}

/// determine the local position based on the cell ID
Position Segmentation::position(const long64& cellID) const {
	return Position(segmentation()->position(cellID));
}

/// determine the cell ID based on the local position
long64 Segmentation::cellID(const Position& localPosition, const Position& globalPosition, const long64& volumeID) const {
	return segmentation()->cellID(localPosition, globalPosition, volumeID);
}
