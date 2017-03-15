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
#include "DD4hep/AlignmentData.h"
#include "DD4hep/MatrixHelpers.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/OpaqueData.h"
#include "DD4hep/Primitives.h"

// ROOT include files
#include "TGeoMatrix.h"
#include <sstream>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Alignments;

/// Copy constructor
Delta::Delta(const Delta& c)
  : translation(c.translation), pivot(c.pivot), rotation(c.rotation), flags(c.flags)
{
}

/// Default destructor
Delta::~Delta()   {
}

/// Assignment operator
Delta& Delta::operator=(const Delta& c)   {
  if ( &c != this )  {
    pivot       = c.pivot;
    translation = c.translation;
    rotation    = c.rotation;
    flags       = c.flags;
  }
  return *this;
}

/// Reset information to identity
void Delta::clear()   {
  flags       = 0;
  pivot       = Pivot();
  translation = Position();
  rotation    = RotationZYX();
}

/// print Conditions object
ostream& operator << (ostream& s, const Delta& data)   {
  string res;
  stringstream str;
  str << "[" << data.translation << "," << data.rotation << "," << data.pivot << "]";
  res = str.str();
  for(size_t i=0; i<res.length(); ++i)
    if ( res[i]=='\n' ) res[i] = ' ';
  return s << res;
}

/// Standard constructor
AlignmentData::AlignmentData()
  : flag(0), magic(magic_word())
{
  InstanceCount::increment(this);
}

/// Copy constructor
AlignmentData::AlignmentData(const AlignmentData& copy)
  : delta(copy.delta), worldTrafo(copy.worldTrafo), worldDelta(copy.worldDelta),
    detectorTrafo(copy.detectorTrafo),
    nodes(copy.nodes), trToWorld(copy.trToWorld), detector(copy.detector),
    placement(copy.placement), flag(copy.flag), magic(magic_word())
{
  InstanceCount::increment(this);
}

/// Default destructor
AlignmentData::~AlignmentData()  {
  InstanceCount::decrement(this);
}

/// Assignment operator necessary due to copy constructor
AlignmentData& AlignmentData::operator=(const AlignmentData& copy)  {
  if ( this != &copy )  {
    delta         = copy.delta;
    worldTrafo    = copy.worldTrafo;
    detectorTrafo = copy.detectorTrafo;
    nodes         = copy.nodes;
    trToWorld     = copy.trToWorld;
    detector      = copy.detector;
    placement     = copy.placement;
    flag          = copy.flag;
  }
  return *this;
}

/// print Conditions object
ostream& operator << (ostream& s, const AlignmentData& data)   {
  stringstream str;
  str << data.delta;
  return s << str.str();
}

/// Transform a point from local coordinates of a given level to global coordinates
Position AlignmentData::localToWorld(const Position& local) const   {
  Position global;
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  worldTrafo.LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
  return global;
}

/// Transformation from local coordinates of the placed volume to the world system
void AlignmentData::localToWorld(const Position& local, Position& global) const   {
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  worldTrafo.LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
}

/// Transformation from local coordinates of the placed volume to the world system
void AlignmentData::localToWorld(const Double_t local[3], Double_t global[3]) const  {
  worldTrafo.LocalToMaster(local, global);
}

/// Transform a point from local coordinates of a given level to global coordinates
Position AlignmentData::worldToLocal(const Position& global) const   {
  Position local;
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  worldTrafo.MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return local;
}

/// Transformation from world coordinates of the local placed volume coordinates
void AlignmentData::worldToLocal(const Position& global, Position& local) const  {
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  worldTrafo.MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
}

/// Transformation from world coordinates of the local placed volume coordinates
void AlignmentData::worldToLocal(const Double_t global[3], Double_t local[3]) const   {
  worldTrafo.MasterToLocal(global, local);
}

/// Transform a point from local coordinates to the coordinates of the DetElement
Position AlignmentData::localToDetector(const Position& local) const   {
  Position global;
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  detectorTrafo.LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
  return global;
}

/// Transformation from local coordinates of the placed volume to the detector system
void AlignmentData::localToDetector(const Position& local, Position& global) const   {
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  detectorTrafo.LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
}

/// Transformation from local coordinates of the placed volume to the detector system
void AlignmentData::localToDetector(const Double_t local[3], Double_t global[3]) const   {
  detectorTrafo.LocalToMaster(local, global);
}

/// Transform a point from local coordinates of the DetElement to global coordinates
Position AlignmentData::detectorToLocal(const Position& global) const   {
  Position local;
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  detectorTrafo.MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return local;
}

/// Transformation from detector element coordinates to the local placed volume coordinates
void AlignmentData::detectorToLocal(const Position& global, Position& local) const   {
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  detectorTrafo.MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
}

/// Transformation from detector element coordinates to the local placed volume coordinates
void AlignmentData::detectorToLocal(const Double_t global[3], Double_t local[3]) const   {
  detectorTrafo.MasterToLocal(global, local);
}

/// Access the ideal/nominal alignment/placement matrix
Alignment AlignmentData::nominal() const   {
  return detector.nominal();
}

#include "DD4hep/ToStream.h"
#include "DD4hep/objects/ConditionsInterna.h"
DD4HEP_DEFINE_OPAQUEDATA_TYPE(Delta)
DD4HEP_DEFINE_OPAQUEDATA_TYPE(AlignmentData)
DD4HEP_DEFINE_CONDITIONS_TYPE_DUMMY(Delta)
DD4HEP_DEFINE_CONDITIONS_TYPE_DUMMY(AlignmentData)

#include "DD4hep/BasicGrammar_inl.h"
DD4HEP_DEFINE_PARSER_GRAMMAR(Delta,eval_none<Delta>)
DD4HEP_DEFINE_PARSER_GRAMMAR(AlignmentData,eval_none<AlignmentData>)
