// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/IDDescriptor.h"
#include "DD4hep/InstanceCount.h"
#include <stdexcept>
#include <cstdlib>
#include <cmath>
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


namespace  {
  void _construct(IDDescriptor::Object* o, const string& dsc) {
    BitField64& bf = *o;
    o->fieldIDs.clear();
    o->fieldMap.clear();
    o->description = dsc ;
    for(size_t i=0; i < bf.size(); ++i)    {
      IDDescriptor::Field f = &bf[i];
      o->fieldIDs.push_back(make_pair(i,f->name()));
      o->fieldMap.push_back(make_pair(f->name(),f));
    }
  }
}

/// Standard constructor
IDDescriptor::Object::Object(const std::string& desc) : TNamed(), BitField64(desc) /*, maxBit(0) */ {
  InstanceCount::increment(this);
}
/// Default destructor
IDDescriptor::Object::~Object()  {
  InstanceCount::decrement(this);
}
 
/// Initializing constructor
IDDescriptor::IDDescriptor(const string& description) 
{
  Object* obj = new Object(description);
  assign(obj,description,"iddescriptor");
  _construct(obj, description);
}

/// Acces string representation
string IDDescriptor::toString() const  {
  if ( isValid() )  {
    return m_element->GetName();
  }
  return "----";
}

std::string  IDDescriptor::fieldDescription() const {
  BitField64* bf = data<Object>() ;
  return bf->fieldDescription() ;
}

/// The total number of encoding bits for this descriptor
unsigned IDDescriptor::maxBit() const  { 
  return data<Object>()->highestBit();
}

/// Access the field-id container 
const IDDescriptor::FieldIDs& IDDescriptor::ids() const    {
  if ( isValid() )  {
    return data<Object>()->fieldIDs;
  }
  throw runtime_error("DD4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Access the fieldmap container 
const IDDescriptor::FieldMap& IDDescriptor::fields() const  { 
  if ( isValid() )  {
    return data<Object>()->fieldMap; 
  }
  throw runtime_error("DD4hep: Attempt to access an invalid IDDescriptor object.");
}

/// Get the field descriptor of one field by name
IDDescriptor::Field IDDescriptor::field(const string& field_name)  const  {
  const FieldMap& m = fields(); // This already checks the object validity
  for(FieldMap::const_iterator i=m.begin(); i!=m.end(); ++i)
    if ( (*i).first == field_name ) return (*i).second;
  throw runtime_error("DD4hep: "+string(name())+": This ID descriptor has no field with the name:"+field_name);
}

/// Get the field descriptor of one field by its identifier
IDDescriptor::Field IDDescriptor::field(size_t identifier)  const   {
  const FieldMap& m = fields(); // This already checks the object validity
  return m[identifier].second;
}

/// Get the field identifier of one field by name
size_t IDDescriptor::fieldID(const string& field_name)  const   {
  const FieldIDs& m = ids(); // This already checks the object validity
  for(FieldIDs::const_iterator i=m.begin(); i!=m.end(); ++i)
    if ( (*i).second == field_name ) return (*i).first;
  throw runtime_error("DD4hep: "+string(name())+": This ID descriptor has no field with the name:"+field_name);
}

/// Encoede a set of volume identifiers (corresponding to this description of course!) to a volumeID.
VolumeID IDDescriptor::encode(const std::vector<VolID>& ids)  const   {
  typedef std::vector<VolID> VolIds;
  VolumeID id = 0;
  for(VolIds::const_iterator i=ids.begin(); i!=ids.end(); ++i)   {
    Field f = field((*i).first);
    id |= f->value((*i).second<<f->offset())<<f->offset();
  }
  return id;
}

/// Decode volume IDs and return filled descriptor with all fields
void IDDescriptor::decodeFields(VolumeID vid, VolIDFields& fields)   {
  fields.clear();
  if ( isValid() )  {
    const vector<BitFieldValue*>& v = data<Object>()->fields();
    for(vector<BitFieldValue*>::const_iterator i=v.begin(); i != v.end(); ++i)
      fields.push_back(VolIDField(*i,(*i)->value(vid)));
    return;
  }
  throw runtime_error("DD4hep: Attempt to access an invalid IDDescriptor object.");
}
