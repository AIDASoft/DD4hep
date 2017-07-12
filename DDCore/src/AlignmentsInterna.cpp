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

// Framework includes
#include "DD4hep/IOV.h"
#include "DD4hep/World.h"
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/DetectorInterna.h"
#include "DD4hep/detail/AlignmentsInterna.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

DD4HEP_INSTANTIATE_HANDLE_NAMED(AlignmentObject,ConditionObject);
DD4HEP_INSTANTIATE_HANDLE_UNNAMED(AlignmentData);

/// Default constructor
AlignmentObject::AlignmentObject()
  : ConditionObject(), alignment_data(0)
{
  InstanceCount::increment(this);
  flags  = Condition::ALIGNMENT_DERIVED;
  AlignmentData& d = data.bind<AlignmentData>();
  alignment_data   = &d;
}

/// Standard constructor
AlignmentObject::AlignmentObject(const string& nam, const string& tit, void* p, size_t len)
  : ConditionObject(nam, tit), alignment_data(0)//, source_key(0)
{
  InstanceCount::increment(this);
  flags  = Condition::ALIGNMENT_DERIVED|Condition::ONSTACK;
  AlignmentData& d = data.bind<AlignmentData>(p,len);
  alignment_data   = &d;
}

/// Standard Destructor
AlignmentObject::~AlignmentObject()  {
  InstanceCount::decrement(this);
}

/// Clear data content on demand.
void AlignmentObject::clear()   {
  AlignmentCondition a(this);
  AlignmentData& d = a.data();
  d.trToWorld = Transform3D();
  d.detectorTrafo.Clear();
  d.worldTrafo.Clear();
  d.nodes.clear();
  flags = Condition::ALIGNMENT_DERIVED;
}


#include "DDParsers/Parsers.h"
#include "DDParsers/ToStream.h"
DD4HEP_DEFINE_PARSER_DUMMY(AlignmentObject)

#include "DD4hep/detail/BasicGrammar_inl.h"
#include "DD4hep/detail/ConditionsInterna.h"
namespace dd4hep {
  template <> bool Grammar<AlignmentObject>::fromString(void*, const std::string&) const {return true;}
  template <> void Grammar<AlignmentObject>::copy(void*, const void*) const {}
  namespace Utils {
    template <> std::ostream& toStream(const AlignmentObject&, std::ostream& s) { return s; }
  }
}
DD4HEP_DEFINE_PARSER_GRAMMAR(AlignmentObject,eval_none<AlignmentObject>)
DD4HEP_DEFINE_CONDITIONS_TYPE(AlignmentObject)

