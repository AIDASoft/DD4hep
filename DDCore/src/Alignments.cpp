//==========================================================================
//  AIDA Detector description implementation 
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
#include "DD4hep/AlignmentData.h"
#include "DD4hep/detail/AlignmentsInterna.h"
#include "DD4hep/detail/ConditionsInterna.h"

// C/C++ include files
#include <sstream>


using namespace std;
using namespace dd4hep;

const string dd4hep::align::Keys::deltaName("alignment_delta");
const dd4hep::Condition::itemkey_type  dd4hep::align::Keys::deltaKey =
  dd4hep::ConditionKey::itemCode("alignment_delta");

const string dd4hep::align::Keys::alignmentName("alignment");
const dd4hep::Condition::itemkey_type dd4hep::align::Keys::alignmentKey =
  dd4hep::ConditionKey::itemCode("alignment");

/// Default constructor
Alignment::Processor::Processor() {
}

/// Initializing constructor to create a new object (Specialized for AlignmentNamedObject)
Alignment::Alignment(const string& nam)  {
  char*   p = (char*)::operator new(sizeof(Object)+sizeof(AlignmentData));
  Object* o = new(p) Object(nam, "alignment", p+sizeof(Object), sizeof(AlignmentData));
  assign(o, nam, "alignment");
  o->hash   = 0;
}

/// Initializing constructor to create a new object (Specialized for AlignmentObject)
AlignmentCondition::AlignmentCondition(const string& nam)   {
  char*   p = (char*)::operator new(sizeof(Object)+sizeof(AlignmentData));
  Object* o = new(p) Object(nam, "alignment", p+sizeof(Object), sizeof(AlignmentData));
  assign(o, nam, "alignment");
  o->hash   = 0;
}

/// Data accessor for the use of decorators
AlignmentData& Alignment::data()              {
  return access()->values();
}

/// Data accessor for the use of decorators
const AlignmentData& Alignment::data() const  {
  return access()->values();
}

/// Access the delta value of the object
const Delta& Alignment::delta() const   {
  return access()->values().delta;
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& Alignment::worldTransformation()  const  {
  return access()->values().worldTransformation();
}

/// Access the alignment/placement matrix with respect to the world
const TGeoHMatrix& Alignment::detectorTransformation() const   {
  return access()->values().detectorTransformation();
}

/// Access to the node list
const vector<PlacedVolume>& Alignment::nodes() const   {
  return access()->values().nodes;
}

/// Transformation from local coordinates of the placed volume to the world system
void Alignment::localToWorld(const Position& local, Position& global) const  {
  return access()->values().localToWorld(local,global);
}

/// Transformation from local coordinates of the placed volume to the world system
void Alignment::localToWorld(const Double_t local[3], Double_t global[3]) const  {
  return access()->values().localToWorld(local,global);
}
/// Transformation from local coordinates of the placed volume to the world system
Position Alignment::localToWorld(const Position& local) const  {
  return access()->values().localToWorld(local);
}

/// Transformation from world coordinates of the local placed volume coordinates
void Alignment::worldToLocal(const Position& global, Position& local) const  {
  return access()->values().worldToLocal(global,local);
}

/// Transformation from world coordinates of the local placed volume coordinates
void Alignment::worldToLocal(const Double_t global[3], Double_t local[3]) const  {
  return access()->values().worldToLocal(global,local);
}

/// Transformation from local coordinates of the placed volume to the world system
Position Alignment::worldToLocal(const Position& global) const  {
  return access()->values().worldToLocal(global);
}

/// Transformation from local coordinates of the placed volume to the detector system
void Alignment::localToDetector(const Position& local, Position& detector) const  {
  return access()->values().localToDetector(local,detector);
}

/// Transformation from local coordinates of the placed volume to the detector system
void Alignment::localToDetector(const Double_t local[3], Double_t detector[3]) const  {
  return access()->values().localToDetector(local,detector);
}

/// Transformation from local coordinates of the placed volume to the world system
Position Alignment::localToDetector(const Position& local) const  {
  return access()->values().localToDetector(local);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
void Alignment::detectorToLocal(const Position& detector, Position& local) const  {
  return access()->values().detectorToLocal(detector,local);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
void Alignment::detectorToLocal(const Double_t detector[3], Double_t local[3]) const  {
  return access()->values().detectorToLocal(detector,local);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
Position Alignment::detectorToLocal(const Position& detector) const  {
  return access()->values().detectorToLocal(detector);
}

/// Access the IOV type
const dd4hep::IOVType& AlignmentCondition::iovType() const   {
  return *(access()->iovType());
}

/// Access the IOV block
const dd4hep::IOV& AlignmentCondition::iov() const   {
  return *(access()->iovData());
}

/// Access the hash identifier
AlignmentCondition::key_type AlignmentCondition::key() const   {
  return access()->hash;
}

/// Data accessor for the use of decorators
AlignmentData& AlignmentCondition::data()              {
  return *(access()->alignment_data);
}

/// Data accessor for the use of decorators
const AlignmentData& AlignmentCondition::data() const  {
  return *(access()->alignment_data);
}

/// Access the delta value of the object
const Delta& AlignmentCondition::delta() const   {
  return access()->alignment_data->delta;
}

/// Check if object is already bound....
bool AlignmentCondition::is_bound()  const  {
  return isValid() ? ptr()->data.is_bound() : false;
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& AlignmentCondition::worldTransformation()  const  {
  return data().worldTransformation();
}

/// Access the alignment/placement matrix with respect to the world
const TGeoHMatrix& AlignmentCondition::detectorTransformation() const   {
  return data().detectorTransformation();
}
