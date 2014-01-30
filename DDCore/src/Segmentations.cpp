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
using DD4hep::DDSegmentation::Parameter;
using DD4hep::DDSegmentation::Parameters;
using DD4hep::DDSegmentation::SegmentationParameter;

/// Standard constructor
Segmentation::Object::Object(BaseSegmentation* s)
  : magic(magic_word()), useForHitPosition(0), segmentation(s) {
  InstanceCount::increment(this);
}

/// Default destructor
Segmentation::Object::~Object() {
  InstanceCount::decrement(this);
  if (segmentation) {
    delete segmentation;
  }
}

/// determine the local position based on the cell ID
DDSegmentation::Position Segmentation::Object::position(const long64& cellID) const {
  return segmentation->position(cellID);
}

/// determine the cell ID based on the local position
long64 Segmentation::Object::cellID(const DDSegmentation::Position& localPosition, const DDSegmentation::Position& globalPosition, const long64& volumeID) const {
  return segmentation->cellID(localPosition, globalPosition, volumeID);
}

/// Access the encoding string
string Segmentation::Object::fieldDescription() const {
  return segmentation->fieldDescription();
}
/// Access the segmentation name
const string& Segmentation::Object::name() const {
  return segmentation->name();
}
/// Set the segmentation name
void Segmentation::Object::setName(const string& value) {
  segmentation->setName(value);
}
/// Access the segmentation type
const string& Segmentation::Object::type() const {
  return segmentation->type();
}
/// Access the description of the segmentation
const string& Segmentation::Object::description() const {
  return segmentation->description();
}
/// Access the underlying decoder
BitField64* Segmentation::Object::decoder() {
  return segmentation->decoder();
}
/// Set the underlying decoder
void Segmentation::Object::setDecoder(BitField64* decoder) {
  segmentation->setDecoder(decoder);
}
/// Access to parameter by name
Parameter Segmentation::Object::parameter(const string& parameterName) const {
  return segmentation->parameter(parameterName);
}
/// Access to all parameters
Parameters Segmentation::Object::parameters() const {
  return segmentation->parameters();
}
/// Set all parameters from an existing set of parameters
void Segmentation::Object::setParameters(const Parameters& parameters) {
  segmentation->setParameters(parameters);
}

/// Constructor to used when creating a new object
Segmentation::Segmentation(const string& type, const string& name) :
  Handle<Implementation>() {
  BaseSegmentation* s = DDSegmentation::SegmentationFactory::instance()->create(type);
  if (s != 0) {
    assign(new Object(s), name, "");
  } else {
    throw runtime_error("FAILED to create segmentation: " + type + ". Missing factory method for: " + type + "!");
  }
}

/// Accessor: Segmentation type
std::string Segmentation::type() const {
  return data<Object>()->type();
}

bool Segmentation::useForHitPosition() const {
  return data<Object>()->useForHitPosition != 0;
}

/// Access to the parameters
Parameters Segmentation::parameters() const {
  return data<Object>()->parameters();
}

/// Access segmentation object
DDSegmentation::Segmentation* Segmentation::segmentation() const {
  return data<Object>()->segmentation;
}

/// determine the local position based on the cell ID
Position Segmentation::position(const long64& cellID) const {
  return Position(segmentation()->position(cellID));
}

/// determine the cell ID based on the local position
long64 Segmentation::cellID(const Position& localPosition, const Position& globalPosition, const long64& volumeID) const {
  return segmentation()->cellID(localPosition, globalPosition, volumeID);
}
