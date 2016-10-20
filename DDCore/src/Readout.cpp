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
#include "DD4hep/Readout.h"
#include "DD4hep/objects/SegmentationsInterna.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/DD4hepUnits.h"
#include "DD4hep/LCDD.h"
#include "DD4hep/Handle.inl"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

/// Copy constructor
HitCollection::HitCollection(const HitCollection& c)
  : name(c.name), key(c.key), key_min(c.key_min), key_max(c.key_max)
{
}

/// Initializing constructor
HitCollection::HitCollection(const string& n, const string& k, long k_min, long k_max)  
  : name(n), key(k), key_min(k_min), key_max(k_max)
{
}

/// Assignment operator
HitCollection& HitCollection::operator=(const HitCollection& c)   {
  if ( this != &c )   {
    name = c.name;
    key = c.key;
    key_min = c.key_min;
    key_max = c.key_max;
  }
  return *this;
}

/// Initializing constructor to create a new object
Readout::Readout(const string& nam) {
  assign(new ReadoutObject(), nam, "readout");
}

/// Access number of hit collections
size_t Readout::numCollections() const   {
  if ( isValid() ) {
    Object& ro = object<Object>();
    return ro.hits.size();
  }
  throw runtime_error("DD4hep: Readout::numCollections: Cannot access object data [Invalid Handle]");
}

/// Access names of hit collections
vector<string> Readout::collectionNames()  const   {
  vector<string> colls;
  if ( isValid() ) {
    Object& ro = object<Object>();
    if ( !ro.hits.empty() )  {
      for(Object::Collections::const_iterator i=ro.hits.begin(); i!=ro.hits.end(); ++i)
        colls.push_back((*i).name);
    }
    return colls;
  }
  throw runtime_error("DD4hep: Readout::collectionsNames: Cannot access object data [Invalid Handle]");
}

/// Access hit collections
vector<const HitCollection*> Readout::collections()  const   {
  vector<const HitCollection*> colls;
  if ( isValid() ) {
    Object& ro = object<Object>();
    if ( !ro.hits.empty() )  {
      for(Object::Collections::const_iterator i=ro.hits.begin(); i!=ro.hits.end(); ++i)
        colls.push_back(&(*i));
    }
    return colls;
  }
  throw runtime_error("DD4hep: Readout::collections: Cannot access object data [Invalid Handle]");
}

/// Assign IDDescription to readout structure
void Readout::setIDDescriptor(const Ref_t& new_descriptor) const {
  if ( isValid() ) {                  // The ID descriptor is NOT owned by the readout!
    if (new_descriptor.isValid()) {   // Do NOT delete!
      data<Object>()->id = new_descriptor;
      Segmentation seg = data<Object>()->segmentation;
      IDDescriptor id  = new_descriptor;
      if ( seg.isValid() ) {
        seg.setDecoder(id.decoder());
      }
      return;
    }
  }
  throw runtime_error("DD4hep: Readout::setIDDescriptor: Cannot assign ID descriptor [Invalid Handle]");
}

/// Access IDDescription structure
IDDescriptor Readout::idSpec() const {
  return object<Object>().id;
}

/// Assign segmentation structure to readout
void Readout::setSegmentation(const Segmentation& seg) const {
  if ( isValid() ) {
    Object& ro = object<Object>();
    Segmentation::Implementation* e = ro.segmentation.ptr();
    if ( e && e != seg.ptr() ) {      // Remember:
      delete e;                       // The segmentation is owned by the readout!
    }                                 // Need to delete the segmentation object
    if ( seg.isValid() ) {
      ro.segmentation = seg;
      return;
    }
  }
  throw runtime_error("DD4hep: Readout::setSegmentation: Cannot assign segmentation [Invalid Handle]");
}

/// Access segmentation structure
Segmentation Readout::segmentation() const {
  return object<Object>().segmentation;
}

