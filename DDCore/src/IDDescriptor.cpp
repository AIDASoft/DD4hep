// $Id$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DD4hep/IDDescriptor.h"
#include <stdexcept>
#include <cstdlib>
#include <cmath>
using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


namespace {
  void split(const string& str, char delim, vector<string>& result)  {
    string t, s, d = str;
    while( !d.empty() )  {
      size_t idx = d.find(delim);
      s = d.substr(0,idx);
      result.push_back(s);
      if ( idx == string::npos ) break;
      d = string(d).substr(idx+1);
    }
  }

  void _construct(IDDescriptor::Object* o, const string& dsc) {
    typedef vector<string>  Elements;
    Elements elements, f;
    IDDescriptor::Field field;
    int pos = 0;

    split(dsc,',',elements);
    o->maxBit = 0;
    o->fieldIDs.clear();
    o->fieldMap.clear();
    for(Elements::const_iterator i=elements.begin();i!=elements.end();++i)  {
      const string& s = *i;
      f.clear();
      split(s,':',f);
      if ( !(f.size() == 2 || f.size() == 3) )
	throw runtime_error("Invalid field descriptor:"+dsc);
      field.first  = f.size() == 3 ? ::atoi(f[1].c_str()) : pos;
      field.second = f.size() == 3 ? ::atoi(f[2].c_str()) : ::atoi(f[1].c_str());
      field.mask   = ~((~0x0ull<<(64-field.second))>>(64-field.second)<<(64-field.first-field.second));
      pos = field.first + ::abs(field.second);
      if ( pos>o->maxBit ) o->maxBit = pos;
      o->fieldIDs.push_back(make_pair(o->fieldMap.size(),f[0]));
      o->fieldMap.push_back(make_pair(f[0],field));
    }
  }
}

/// Initializing constructor
IDDescriptor::IDDescriptor(const string& description) 
{
  Value<TNamed,Object>* obj = new Value<TNamed,Object>();
  assign(obj,description,"iddescriptor");
  _construct(obj, description);
}

/// The total number of encoding bits for this descriptor
int IDDescriptor::maxBit() const  { 
  return data<Object>()->maxBit;
}

/// Access the field-id container 
const IDDescriptor::FieldIDs& IDDescriptor::ids() const    {
  if ( isValid() )  {
    return data<Object>()->fieldIDs;
  }
  throw runtime_error("Attempt to access an invalid IDDescriptor object.");
}

/// Access the fieldmap container 
const IDDescriptor::FieldMap& IDDescriptor::fields() const  { 
  if ( isValid() )  {
    return data<Object>()->fieldMap; 
  }
  throw runtime_error("Attempt to access an invalid IDDescriptor object.");
}

/// Get the field descriptor of one field by name
IDDescriptor::Field IDDescriptor::field(const string& field_name)  const  {
  const FieldMap& m = fields(); // This already checks the object validity
  for(FieldMap::const_iterator i=m.begin(); i!=m.end(); ++i)
    if ( (*i).first == field_name ) return (*i).second;
  throw runtime_error(string(name())+": This ID descriptor has now field with the name:"+field_name);
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
  throw runtime_error(string(name())+": This ID descriptor has now field with the name:"+field_name);
}
