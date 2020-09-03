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

#include "DD4hep/IDDescriptor.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Printout.h"
#include <stdexcept>
#include <cstdlib>
#include <cmath>
using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace {
  void _construct(IDDescriptor::Object* o, const string& dsc) {
    BitFieldCoder& bf = o->decoder;
    o->fieldIDs.clear();
    o->fieldMap.clear();
    o->description = dsc;
    for (size_t i = 0; i < bf.size(); ++i) {
      const BitFieldElement* f = &bf[i];
      o->fieldIDs.emplace_back(i, f->name());
      o->fieldMap.emplace_back(f->name(), f);
    }
  }
}

/// Initializing constructor
IDDescriptor::IDDescriptor(const string& nam, const string& description) {
  Object* obj = new Object(description);
  assign(obj, nam, "iddescriptor");
  _construct(obj, description);
}

/// Re-build object in place
void IDDescriptor::rebuild(const string& description)   {
  Object* p = ptr();
  string  dsc = description;
  p->decoder.~BitFieldCoder();
  new(&p->decoder) BitFieldCoder(dsc);
  _construct(p, dsc);
}

/// Acces string representation
string IDDescriptor::toString() const {
  if ( isValid() ) {
    return m_element->GetName();
  }
  return "----";
}

std::string IDDescriptor::fieldDescription() const {
  BitFieldCoder& bf = data<Object>()->decoder;
  return bf.fieldDescription();
}

/// The total number of encoding bits for this descriptor
unsigned IDDescriptor::maxBit() const {
  return data<Object>()->decoder.highestBit();
}

/// Access the field-id container
const IDDescriptor::FieldIDs& IDDescriptor::ids() const {
  if ( isValid() ) {
    return data<Object>()->fieldIDs;
  }
  throw runtime_error("dd4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Access the fieldmap container
const IDDescriptor::FieldMap& IDDescriptor::fields() const {
  if ( isValid() ) {
    return data<Object>()->fieldMap;
  }
  throw runtime_error("dd4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Get the field descriptor of one field by name
const BitFieldElement* IDDescriptor::field(const string& field_name) const {
  const FieldMap& fm = fields();   // This already checks the object validity
  for (const auto& i : fm )
    if (i.first == field_name)
      return i.second;
  except("IDDescriptor","dd4hep: %s: This ID descriptor has no field with the name: %s",
         name(),field_name.c_str());
  throw runtime_error("dd4hep");  // Never called. Simply make the compiler happy!
}

/// Get the field descriptor of one field by its identifier
const BitFieldElement* IDDescriptor::field(size_t identifier) const {
  const FieldMap& fm = fields();   // This already checks the object validity
  return fm[identifier].second;
}

/// Get the field identifier of one field by name
size_t IDDescriptor::fieldID(const string& field_name) const {
  const FieldIDs& fm = ids();   // This already checks the object validity
  for (const auto& i : fm )
    if (i.second == field_name)
      return i.first;
  except("IDDescriptor","dd4hep: %s: This ID descriptor has no field with the name: %s",
         name(),field_name.c_str());
  throw runtime_error("dd4hep");  // Never called. Simply make the compiler happy!
}

/// Compute the submask for a given set of volume IDs
VolumeID IDDescriptor::get_mask(const std::vector<std::pair<std::string, int> >& id_vector) const   {
  VolumeID mask = 0ULL;
  for (const auto& i : id_vector )   {
    const auto* fld = field(i.first);
    mask |= fld->mask();
  }
  return mask;
}

/// Encode a set of volume identifiers (corresponding to this description of course!) to a volumeID.
VolumeID IDDescriptor::encode(const std::vector<std::pair<std::string, int> >& id_vector) const
{
  VolumeID id = 0;
  //const PlacedVolume::VolIDs* ids = (const PlacedVolume::VolIDs*)&id_vector;
  //printout(INFO,"IDDescriptor","VolIDs: %s",ids->str().c_str());
  for (const auto& i : id_vector )  {
    const BitFieldElement* fld = field(i.first);
    int      off = fld->offset();
    VolumeID val = i.second;
    id |= ((fld->value(val << off) << off)&fld->mask());
  }
  return id;
}

/// Encode a set of volume identifiers to a volumeID with the system ID on the top bits
VolumeID IDDescriptor::encode_reverse(const std::vector<std::pair<std::string, int> >& id_vector) const
{
  return detail::reverseBits<VolumeID>(encode(id_vector));
}

/// Decode volume IDs and return filled descriptor with all fields
void IDDescriptor::decodeFields(VolumeID vid,
                                vector<pair<const BitFieldElement*, VolumeID> >& flds)  const
{
  const vector<BitFieldElement>& v = access()->decoder.fields();
  flds.clear();
  for (auto& f : v )
    flds.emplace_back(&f, f.value(vid));
}

/// Decode volume IDs and return string reprensentation for debugging purposes
string IDDescriptor::str(VolumeID vid)   const {
  const vector<BitFieldElement>& v = access()->decoder.fields();
  stringstream str;
  for (auto& f : v )
    str << f.name() << ":" << setw(4) << setfill('0') << hex << right << f.value(vid)
        << left << dec << " ";
  return str.str().substr(0,str.str().length()-1);
}

/// Decode volume IDs and return string reprensentation for debugging purposes
string IDDescriptor::str(VolumeID vid, VolumeID mask)   const {
  const vector<BitFieldElement>& v = access()->decoder.fields();
  stringstream str;
  for (auto& f : v )  {
    if ( 0 == (mask&f.mask()) ) continue;
    str << f.name() << ":" << setw(4) << setfill('0') << hex << right << f.value(vid)
        << left << dec << " ";
  }
  return str.str().substr(0,str.str().length()-1);
}

/// Access the BitFieldCoder object
BitFieldCoder* IDDescriptor::decoder()   const   {
  return &(data<Object>()->decoder);
}
