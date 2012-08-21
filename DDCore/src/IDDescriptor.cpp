// $Id:$
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
      pos = field.first + ::abs(field.second);
      if ( pos>o->maxBit ) o->maxBit = pos;
      o->fieldIDs.insert(make_pair(o->fieldMap.size(),f[0]));
      o->fieldMap.insert(make_pair(f[0],field));
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

int IDDescriptor::maxBit() const             
{ return data<Object>()->maxBit;   }

const IDDescriptor::FieldIDs& IDDescriptor::ids() const    
{ return data<Object>()->fieldIDs; }

const IDDescriptor::FieldMap& IDDescriptor::fields() const 
{ return data<Object>()->fieldMap; }
