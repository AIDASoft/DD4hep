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
#include "DD4hep/IDDescriptor.h"
#include "DD4hep/detail/Handle.inl"
#include "DD4hep/detail/ObjectsInterna.h"
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Volumes.h"
#include "DD4hep/Printout.h"

// C/C++ include files
#include <stdexcept>
#include <cstdlib>
#include <cmath>

using namespace dd4hep;

namespace {
  void _construct(IDDescriptor::Object* o, const std::string& dsc) {
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
IDDescriptor::IDDescriptor(const std::string& nam, const std::string& description) {
  Object* obj = new Object(description);
  assign(obj, nam, "iddescriptor");
  _construct(obj, description);
}

/// Re-build object in place
void IDDescriptor::rebuild(const std::string& description)   {
  Object* p = ptr();
  std::string  dsc = description;
  p->decoder.~BitFieldCoder();
  new(&p->decoder) BitFieldCoder(dsc);
  _construct(p, dsc);
}

/// Acces std::string representation
std::string IDDescriptor::toString() const {
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
  except("IDDescriptor","dd4hep: Attempt to access an invalid IDDescriptor object.");
  throw std::runtime_error("dd4hep");  // Never called. Simply make the compiler happy!
}

/// Access the fieldmap container
const IDDescriptor::FieldMap& IDDescriptor::fields() const {
  if ( isValid() ) {
    return data<Object>()->fieldMap;
  }
  except("IDDescriptor","dd4hep: Attempt to access an invalid IDDescriptor object.");
  throw std::runtime_error("dd4hep");  // Never called. Simply make the compiler happy!
}

/// Get the field descriptor of one field by name
const BitFieldElement* IDDescriptor::field(const std::string& field_name) const {
  const FieldMap& fm = fields();   // This already checks the object validity
  for (const auto& i : fm )
    if (i.first == field_name)
      return i.second;
  except("IDDescriptor","dd4hep: %s: This ID descriptor has no field with the name: %s",
         name(),field_name.c_str());
  throw std::runtime_error("dd4hep");  // Never called. Simply make the compiler happy!
}

/// Get the field descriptor of one field by its identifier
const BitFieldElement* IDDescriptor::field(size_t identifier) const {
  const FieldMap& fm = fields();   // This already checks the object validity
  return fm[identifier].second;
}

/// Get the field identifier of one field by name
std::size_t IDDescriptor::fieldID(const std::string& field_name) const {
  const FieldIDs& fm = ids();   // This already checks the object validity
  for (const auto& i : fm )
    if (i.second == field_name)
      return i.first;
  except("IDDescriptor","dd4hep: %s: This ID descriptor has no field with the name: %s",
         name(),field_name.c_str());
  throw std::runtime_error("dd4hep");  // Never called. Simply make the compiler happy!
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
VolumeID IDDescriptor::encode(const std::vector<std::pair<std::string, int> >& id_vector) const  {
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

/// Encode partial volume identifiers to a volumeID.
VolumeID IDDescriptor::encode(const Field* fld, VolumeID value)  {
  if ( fld )   {
    int off = fld->offset();
    return ((fld->value(value << off) << off)&fld->mask());
  }
  except("IDDescriptor","dd4hep: %s: Cannot encode value with void Field reference.");
  return 0UL;
}

/// Encode a set of volume identifiers to a volumeID with the system ID on the top bits
VolumeID IDDescriptor::encode_reverse(const std::vector<std::pair<std::string, int> >& id_vector) const
{
  return detail::reverseBits<VolumeID>(encode(id_vector));
}

/// Decode volume IDs and return filled descriptor with all fields
void IDDescriptor::decodeFields(VolumeID vid,
                                std::vector<std::pair<const BitFieldElement*, VolumeID> >& flds)  const
{
  const std::vector<BitFieldElement>& v = access()->decoder.fields();
  flds.clear();
  for (auto& f : v )
    flds.emplace_back(&f, f.value(vid));
}

/// Decode volume IDs and return string reprensentation for debugging purposes
std::string IDDescriptor::str(VolumeID vid)   const {
  const std::vector<BitFieldElement>& v = access()->decoder.fields();
  std::stringstream str;
  for (auto& f : v )
    str << f.name()  << ":" << std::setw(4) << std::setfill('0')
        << std::hex  << std::right << f.value(vid)
        << std::left << std::dec << " ";
  return str.str().substr(0, str.str().length()-1);
}

/// Decode volume IDs and return string reprensentation for debugging purposes
std::string IDDescriptor::str(VolumeID vid, VolumeID mask)   const {
  const std::vector<BitFieldElement>& v = access()->decoder.fields();
  std::stringstream str;
  for (auto& f : v )  {
    if ( 0 == (mask&f.mask()) ) continue;
    str << f.name()  << ":" << std::setw(4) << std::setfill('0')
        << std::hex  << std::right << f.value(vid)
        << std::left << std::dec << " ";
  }
  return str.str().substr(0, str.str().length()-1);
}

/// Access the BitFieldCoder object
BitFieldCoder* IDDescriptor::decoder()   const   {
  return &(data<Object>()->decoder);
}
