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
#include "DD4hep/Handle.inl"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;
using DD4hep::DDSegmentation::Parameter;
using DD4hep::DDSegmentation::Parameters;
using DD4hep::DDSegmentation::SegmentationParameter;

DD4HEP_INSTANTIATE_HANDLE_UNNAMED(SegmentationObject);

/// Standard constructor
SegmentationObject::SegmentationObject(BaseSegmentation* s)
  : magic(magic_word()), useForHitPosition(0), segmentation(s) {
  InstanceCount::increment(this);
}

/// Default destructor
SegmentationObject::~SegmentationObject() {
  InstanceCount::decrement(this);
  if (segmentation) {
    delete segmentation;
  }
}

/// determine the local position based on the cell ID
DDSegmentation::Vector3D SegmentationObject::position(const long64& cell) const {
  return segmentation->position(cell);
}

/// determine the cell ID based on the local position
long64 SegmentationObject::cellID(const DDSegmentation::Vector3D& localPosition, const DDSegmentation::Vector3D& globalPosition, const long64& volID) const {
  return segmentation->cellID(localPosition, globalPosition, volID);
}

/// Access the encoding string
string SegmentationObject::fieldDescription() const {
  return segmentation->fieldDescription();
}
/// Access the segmentation name
const string& SegmentationObject::name() const {
  return segmentation->name();
}
/// Set the segmentation name
void SegmentationObject::setName(const string& value) {
  segmentation->setName(value);
}

/// Access the segmentation type
const string& SegmentationObject::type() const {
  return segmentation->type();
}

/// Access the description of the segmentation
const string& SegmentationObject::description() const {
  return segmentation->description();
}

/// Access the underlying decoder
BitField64* SegmentationObject::decoder() {
  return segmentation->decoder();
}

/// Set the underlying decoder
void SegmentationObject::setDecoder(BitField64* ptr_decoder) {
  segmentation->setDecoder(ptr_decoder);
}

/// Access to parameter by name
Parameter SegmentationObject::parameter(const string& parameterName) const {
  return segmentation->parameter(parameterName);
}

/// Access to all parameters
Parameters SegmentationObject::parameters() const {
  return segmentation->parameters();
}

/// Set all parameters from an existing set of parameters
void SegmentationObject::setParameters(const Parameters& params) {
  segmentation->setParameters(params);
}

/// Constructor to used when creating a new object
Segmentation::Segmentation(const string& typ, const string& nam) :
  Handle<Implementation>() {
  BaseSegmentation* s = DDSegmentation::SegmentationFactory::instance()->create(typ);
  if (s != 0) {
    assign(new Object(s), nam, "");
  } else {
    throw runtime_error("FAILED to create segmentation: " + typ + ". Missing factory method for: " + typ + "!");
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
Position Segmentation::position(const long64& cell) const {
  return Position(segmentation()->position(cell));
}

/// determine the cell ID based on the local position
long64 Segmentation::cellID(const Position& localPosition, const Position& globalPosition, const long64& volID) const {
  return segmentation()->cellID(localPosition, globalPosition, volID);
}
