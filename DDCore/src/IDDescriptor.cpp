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

#include "DD4hep/Handle.inl"
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/objects/ObjectsInterna.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Printout.h"
#include <stdexcept>
#include <cstdlib>
#include <cmath>
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

namespace {
  void _construct(IDDescriptor::Object* o, const string& dsc) {
    BitField64& bf = *o;
    o->fieldIDs.clear();
    o->fieldMap.clear();
    o->description = dsc;
    for (size_t i = 0; i < bf.size(); ++i) {
      IDDescriptor::Field f = &bf[i];
      o->fieldIDs.push_back(make_pair(i, f->name()));
      o->fieldMap.push_back(make_pair(f->name(), f));
    }
  }
}

/// Initializing constructor
IDDescriptor::IDDescriptor(const string& description) {
  Object* obj = new Object(description);
  assign(obj, description, "iddescriptor");
  _construct(obj, description);
}

/// Acces string representation
string IDDescriptor::toString() const {
  if (isValid()) {
    return m_element->GetName();
  }
  return "----";
}

std::string IDDescriptor::fieldDescription() const {
  BitField64* bf = data<Object>();
  return bf->fieldDescription();
}

/// The total number of encoding bits for this descriptor
unsigned IDDescriptor::maxBit() const {
  return data<Object>()->highestBit();
}

/// Access the field-id container
const IDDescriptor::FieldIDs& IDDescriptor::ids() const {
  if ( isValid() ) {
    return data<Object>()->fieldIDs;
  }
  throw runtime_error("DD4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Access the fieldmap container
const IDDescriptor::FieldMap& IDDescriptor::fields() const {
  if ( isValid() ) {
    return data<Object>()->fieldMap;
  }
  throw runtime_error("DD4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Get the field descriptor of one field by name
IDDescriptor::Field IDDescriptor::field(const string& field_name) const {
  const FieldMap& m = fields();   // This already checks the object validity
  for (const auto& i : m )
    if (i.first == field_name)
      return i.second;
  except("IDDescriptor","DD4hep: %s: This ID descriptor has no field with the name: %s",
         name(),field_name.c_str());
  throw runtime_error("DD4hep");  // Never called. Simply make the compiler happy!
}

/// Get the field descriptor of one field by its identifier
IDDescriptor::Field IDDescriptor::field(size_t identifier) const {
  const FieldMap& m = fields();   // This already checks the object validity
  return m[identifier].second;
}

/// Get the field identifier of one field by name
size_t IDDescriptor::fieldID(const string& field_name) const {
  const FieldIDs& m = ids();   // This already checks the object validity
  for (const auto& i : m )
    if (i.second == field_name)
      return i.first;
  except("IDDescriptor","DD4hep: %s: This ID descriptor has no field with the name: %s",
         name(),field_name.c_str());
  throw runtime_error("DD4hep");  // Never called. Simply make the compiler happy!
}

/// Encode a set of volume identifiers (corresponding to this description of course!) to a volumeID.
VolumeID IDDescriptor::encode(const std::vector<VolID>& id_vector) const {
  VolumeID id = 0;
  for (const auto& i : id_vector )  {
    Field f = field(i.first);
    int offset = f->offset();
    id |= ((f->value(i.second<<offset) << offset)&f->mask());
  }
  return id;
}

/// Decode volume IDs and return filled descriptor with all fields
void IDDescriptor::decodeFields(VolumeID vid, VolIDFields& flds) {
  flds.clear();
  if ( isValid() ) {
    const vector<BitFieldValue*>& v = ptr()->fields();
    for (auto f : v )
      flds.push_back(VolIDField(f, f->value(vid)));
    return;
  }
  except("IDDescriptor","DD4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Access the BitField64 object
BitField64* IDDescriptor::decoder() {
  return data<Object>();
}
