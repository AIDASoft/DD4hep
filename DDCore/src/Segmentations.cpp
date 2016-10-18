// $Id$
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/Segmentations.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Handle.inl"

// C/C++ include files
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
  : magic(magic_word()), useForHitPosition(0),
    detector(0), sensitive(0), segmentation(s)
{
  //s->backLink = this;
  //this->backLink = this;
  InstanceCount::increment(this);
}

/// Default destructor
SegmentationObject::~SegmentationObject() {
  InstanceCount::decrement(this);
  if (segmentation) {
    delete segmentation;
  }
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

/// Determine the local position based on the cell ID
Position SegmentationObject::position(const CellID& cell) const  {
  return Position(segmentation->position(cell));
}

/// Determine the cell ID based on the position
CellID SegmentationObject::cellID(const Position& local,
                                  const Position& global,
                                  const VolumeID& volID) const  {
  return segmentation->cellID(local, global, volID);
}

/// Determine the volume ID from the full cell ID by removing all local fields
VolumeID SegmentationObject::volumeID(const CellID& cell) const   {
  return segmentation->volumeID(cell);
}

/// Calculates the neighbours of the given cell ID and adds them to the list of neighbours
void SegmentationObject::neighbours(const CellID& cell, std::set<CellID>& nb) const   {
  segmentation->neighbours(cell, nb);
}

/// Constructor to used when creating a new object
Segmentation::Segmentation(const string& typ, const string& nam) :
  Handle<Implementation>() {
  BaseSegmentation* s = DDSegmentation::SegmentationFactory::instance()->create(typ);
  if (s != 0) {
    assign(new Object(s), nam, "");
    if ( !nam.empty() ) s->setName(nam);
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

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {
  /// Namespace for the geometry part of the AIDA detector description toolkit
  namespace Geometry {

    /// Access the concrete underlying segmentation implementation from DDSegmentation
    template <typename T> T*
    Segmentation::get(const Object* obj)   {
      if ( obj )  {
        DDSegmentation::Segmentation* seg = obj->segmentation;
        T* dd_seg = dynamic_cast<T*>(seg);
        if ( dd_seg )  {
          return dd_seg;
        }
        except("CartesianGridXY",
               "Cannot convert segmentation:%s to CartesianGridXY.",
               obj->name().c_str());
      }
      return 0;
    }
  } /* End namespace Geometry              */
} /* End namespace DD4hep                */

#define IMPLEMENT_SEGMENTATION_HANDLE(X)                                \
  DD4HEP_INSTANTIATE_HANDLE_UNNAMED(DDSegmentation::X);                 \
  namespace DD4hep { namespace Geometry {                               \
      template DDSegmentation::X*                                       \
      Segmentation::get<DDSegmentation::X>(const Object* obj); }}

#include "DDSegmentation/CartesianGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridXY)

#include "DDSegmentation/CartesianGridXZ.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridXZ)

#include "DDSegmentation/CartesianGridYZ.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridYZ)

#include "DDSegmentation/CartesianGridXYZ.h"
IMPLEMENT_SEGMENTATION_HANDLE(CartesianGridXYZ)

#include "DDSegmentation/TiledLayerGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(TiledLayerGridXY)

#include "DDSegmentation/MegatileLayerGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(MegatileLayerGridXY)

#include "DDSegmentation/WaferGridXY.h"
IMPLEMENT_SEGMENTATION_HANDLE(WaferGridXY)

#include "DDSegmentation/PolarGridRPhi.h"
IMPLEMENT_SEGMENTATION_HANDLE(PolarGridRPhi)

#include "DDSegmentation/PolarGridRPhi2.h"
IMPLEMENT_SEGMENTATION_HANDLE(PolarGridRPhi2)

#include "DDSegmentation/ProjectiveCylinder.h"
IMPLEMENT_SEGMENTATION_HANDLE(ProjectiveCylinder)

