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


void DD4hep::split(const string& str, char delim, vector<string>& result)  {
  string t, s, d = str;
  while( !d.empty() )  {
    size_t idx = d.find(delim);
    s = d.substr(0,idx);
    result.push_back(s);
    if ( idx == string::npos ) break;
    d = string(d).substr(idx+1);
  }
}

IDDescriptor::IDDescriptor(const string& dsc) : m_maxBit(0) {
  construct(dsc);
}

IDDescriptor::IDDescriptor() : m_maxBit(0) {
}

IDDescriptor::~IDDescriptor()   {
}

void IDDescriptor::construct(const std::string& dsc)   {
  typedef vector<string>  Elements;
  Elements elements, f;
  Field field;
  int pos = 0;

  split(dsc,',',elements);
  m_fieldIDs.clear();
  m_fieldMap.clear();
  for(Elements::const_iterator i=elements.begin();i!=elements.end();++i)  {
    const string& s = *i;
    f.clear();
    split(s,':',f);
    if ( !(f.size() == 2 || f.size() == 3) )
      throw runtime_error("Invalid field descriptor:"+dsc);
    field.first  = f.size() == 3 ? ::atoi(f[1].c_str()) : pos;
    field.second = f.size() == 3 ? ::atoi(f[2].c_str()) : ::atoi(f[1].c_str());
    pos = field.first + ::abs(field.second);
    if ( pos>m_maxBit ) m_maxBit = pos;
    m_fieldIDs.insert(make_pair(m_fieldMap.size(),f[0]));
    m_fieldMap.insert(make_pair(f[0],field));
  }
}

