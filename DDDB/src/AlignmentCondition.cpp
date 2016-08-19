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

// Framework includes
#include "DD4hep/DetectorTools.h"
//#include "DDCond/AlignmentCondition.h"
#include "AlignmentCondition.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

using namespace DD4hep::Conditions;
using DD4hep::Geometry::DetectorTools::ElementPath;
using DD4hep::Geometry::DetectorTools::PlacementPath;

/// Standard constructor
AlignmentConditionData::AlignmentConditionData()
  : flag(0), magic(magic_word())
{
}

/// Default destructor
AlignmentConditionData::~AlignmentConditionData()  {
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& AlignmentConditionData::worldTransformation()   const  {
  if ( (flag&HAVE_WORLD_TRAFO) == 0 ) {
    PlacementPath nodes;
    flag |= HAVE_WORLD_TRAFO;
    //Geometry::DetectorTools::placementPath(condition->detector, nodes);
    //Geometry::DetectorTools::placementTrafo(nodes,false,worldTrafo);
    throw std::runtime_error("not implemented");
  }
  return worldTrafo;
}

/// Create cached matrix to transform to parent coordinates
const TGeoHMatrix& AlignmentConditionData::parentTransformation()  const  {
  if ( (flag&HAVE_PARENT_TRAFO) == 0 ) {
    PlacementPath nodes;
    DetElement det;// = condition->detector;
    flag |= HAVE_PARENT_TRAFO;
    Geometry::DetectorTools::placementPath(det.parent(), det, nodes);
    Geometry::DetectorTools::placementTrafo(nodes,false,parentTrafo);
  }
  return parentTrafo;
}

/// Default constructor
AlignmentCondition::AlignmentCondition() : Condition()  {
}

/// Copy constructor
AlignmentCondition::AlignmentCondition(const Condition& c) : Condition(c) {
}

/// Copy constructor
AlignmentCondition::AlignmentCondition(const AlignmentCondition& c) : Condition(c) {
}

/// Initializing constructor
AlignmentCondition::AlignmentCondition(Object* p) : Condition(p) {
}

/// Assignment operator
AlignmentCondition& AlignmentCondition::operator=(const AlignmentCondition& c)  {
  if ( this != &c ) this->m_element = c.m_element;
  return *this;
}

/// Create cached matrix to transform to world coordinates
const TGeoHMatrix& AlignmentCondition::worldTransformation() const   {
  return get<AlignmentConditionData>().worldTransformation();
}

/// Create cached matrix to transform to parent coordinates
const TGeoHMatrix& AlignmentCondition::parentTransformation() const   {
  return get<AlignmentConditionData>().parentTransformation();
}

/// Transformation from local coordinates of the placed volume to the world system
bool AlignmentCondition::localToWorld(const Position& local, Position& global) const  {
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  worldTransformation().LocalToMaster(local_point, master_point);
  global.SetCoordinates(master_point);
  return true;
}

/// Transformation from local coordinates of the placed volume to the parent system
bool AlignmentCondition::localToParent(const Position& local, Position& parent) const   {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  Double_t master_point[3] = { 0, 0, 0 }, local_point[3] = { local.X(), local.Y(), local.Z() };
  parentTransformation().LocalToMaster(local_point, master_point);
  parent.SetCoordinates(master_point);
  return true;
}

/// Transformation from world coordinates of the local placed volume coordinates
bool AlignmentCondition::worldToLocal(const Position& global, Position& local) const   {
  // If the path is unknown an exception will be thrown inside worldTransformation() !
  Double_t master_point[3] = { global.X(), global.Y(), global.Z() }, local_point[3] = { 0, 0, 0 };
  worldTransformation().MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return true;
}

/// Transformation from world coordinates of the local placed volume coordinates
bool AlignmentCondition::parentToLocal(const Position& parent, Position& local) const  {
  // If the path is unknown an exception will be thrown inside parentTransformation() !
  Double_t master_point[3] = { parent.X(), parent.Y(), parent.Z() }, local_point[3] = { 0, 0, 0 };
  parentTransformation().MasterToLocal(master_point, local_point);
  local.SetCoordinates(local_point);
  return true;
}

std::ostream& operator<<(std::ostream& s, const AlignmentConditionData&)  {
  return s; 
}

#include "DD4hep/ToStream.h"
#include "DD4hep/objects/ConditionsInterna.h"
DD4HEP_DEFINE_CONDITIONS_TYPE_DUMMY(AlignmentConditionData)

#include "DD4hep/objects/BasicGrammar_inl.h"
DD4HEP_DEFINE_PARSER_GRAMMAR(AlignmentConditionData,eval_none<AlignmentConditionData>)

