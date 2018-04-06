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
//
// DDDB is a detector description convention developed by the LHCb experiment.
// For further information concerning the DTD, please see:
// http://lhcb-comp.web.cern.ch/lhcb-comp/Frameworks/DetDesc/Documents/lhcbDtd.pdf
//
//==========================================================================

// Framework includes
#include "DDDB/DDDBHelper.h"
#include "DDDB/DDDBConversion.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::DDDB;

namespace {
  struct ByName {
    const string& n;
    ByName(const string& s) : n(s) {}
    bool operator() (const pair<string, VisAttr>& o) const  {
      return o.first == n;
    }
  };
}

/// Standard constructor
DDDBHelper::DDDBHelper(Detector& dsc) 
 : m_description(dsc), m_xmlReader(0), m_detDesc(0)
{
}

/// Default destructor
DDDBHelper::~DDDBHelper()    {
  detail::deletePtr(m_detDesc);
  m_visAttrs.clear();
}

/// Attach detectorDescription information
void DDDBHelper::setDetectorDescription(dddb* geo)   {
  detail::deletePtr(m_detDesc);
  m_detDesc = geo;
}

/// Access visualization attribute for a given volume by path
VisAttr DDDBHelper::visAttr(const std::string& path)  const   {
  VisAttrs::const_iterator i = std::find_if(m_visAttrs.begin(), m_visAttrs.end(), ByName(path));
  if ( i == m_visAttrs.end() )  {
    for( i=m_visAttrs.begin(); i != m_visAttrs.end(); ++i)  {
      size_t idx = path.find((*i).first);
      if ( idx == 0 )  {
        return (*i).second;
      }
    }
    return VisAttr();
  }
  return (*i).second;
}

/// Add visualization attribute
void DDDBHelper::addVisAttr(const std::string& path, const std::string attr_name)    {
  VisAttr attr = m_description.visAttributes(attr_name);
  if ( attr.isValid() )   {
    addVisAttr(path, attr);
  }
}

/// Add visualization attribute
void DDDBHelper::addVisAttr(const std::string& path, VisAttr attr)    {
  if ( attr.isValid() )   {
    VisAttrs::const_iterator i = std::find_if(m_visAttrs.begin(), m_visAttrs.end(), ByName(path));
    if ( i == m_visAttrs.end() )  {
      m_visAttrs.push_back(make_pair(path, attr));
    }
  }
}

/// Add new conditions entry
bool DDDBHelper::addConditionEntry(const std::string& key, DetElement det, const std::string& item)    {
  return m_detCond.insert(make_pair(key,make_pair(det,item))).second;
}

/// Access conditions entry
std::pair<DetElement,std::string> DDDBHelper::getConditionEntry(const std::string& key)  const    {
  auto i = m_detCond.find(key);
  if ( i != m_detCond.end() )
    return (*i).second;
  return make_pair(DetElement(),"");
}

