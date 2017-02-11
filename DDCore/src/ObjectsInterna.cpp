// $Id: $
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

#include "DD4hep/Handle.inl"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Objects.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/objects/SegmentationsInterna.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

DD4HEP_INSTANTIATE_HANDLE_NAMED(VisAttrObject);

/// Standard constructor
VisAttrObject::VisAttrObject()
  : magic(magic_word()), col(0), color(0), alpha(0), drawingStyle(VisAttr::SOLID), lineStyle(VisAttr::SOLID), showDaughters(true), visible(true) {
  InstanceCount::increment(this);
}

/// Default destructor
VisAttrObject::~VisAttrObject() {
  InstanceCount::decrement(this);
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(HeaderObject);

/// Standard constructor
HeaderObject::HeaderObject()
  : NamedObject() {
  InstanceCount::increment(this);
}

/// Default destructor
HeaderObject::~HeaderObject() {
  InstanceCount::decrement(this);
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(ConstantObject);

/// Standard constructor
ConstantObject::ConstantObject(const string& nam, const string& val, const string& typ)
  : NamedObject(nam.c_str(), val.c_str()) {
  dataType = typ;
  InstanceCount::increment(this);
}

/// Default destructor
ConstantObject::~ConstantObject() {
  InstanceCount::decrement(this);
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(RegionObject);

/// Standard constructor
RegionObject::RegionObject()
  : magic(magic_word()), threshold(10.0), cut(10.0), store_secondaries(false),
    use_default_cut(true), was_threshold_set(false)
{
  InstanceCount::increment(this);
}

/// Default destructor
RegionObject::~RegionObject() {
  InstanceCount::decrement(this);
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(LimitSetObject);

/// Standard constructor
LimitSetObject::LimitSetObject() {
  InstanceCount::increment(this);
}

/// Default destructor
LimitSetObject::~LimitSetObject() {
  InstanceCount::decrement(this);
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(ReadoutObject);

/// Standard constructor
ReadoutObject::ReadoutObject() {
  InstanceCount::increment(this);
}

/// Default destructor
ReadoutObject::~ReadoutObject() {
  //destroyHandle(segmentation);
  InstanceCount::decrement(this);
  if ( segmentation.isValid() )  {
    delete segmentation.ptr();
    segmentation = Segmentation();
  }
}

DD4HEP_INSTANTIATE_HANDLE_NAMED(IDDescriptorObject);

/// Standard constructor
IDDescriptorObject::IDDescriptorObject()
  : NamedObject(), BitField64() /*, maxBit(0) */{
  InstanceCount::increment(this);
}

/// Standard constructor
IDDescriptorObject::IDDescriptorObject(const std::string& desc)
  : NamedObject(), BitField64(desc) /*, maxBit(0) */{
  InstanceCount::increment(this);
}

/// Default destructor
IDDescriptorObject::~IDDescriptorObject() {
  InstanceCount::decrement(this);
}
