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
using namespace DD4hep::DDDB;

namespace {
  struct ByName {
    const string& n;
    ByName(const string& s) : n(s) {}
    bool operator() (const pair<string, DD4hep::Geometry::VisAttr>& o) const  {
      return o.first == n;
    }
  };
}

/// Standard constructor
DDDBHelper::DDDBHelper(Geometry::LCDD& l) 
 : m_lcdd(l), m_xmlReader(0), m_geometry(0)
{
}

/// Default destructor
DDDBHelper::~DDDBHelper()    {
  deletePtr(m_geometry);
  m_visAttrs.clear();
}

/// Attach geometry information
void DDDBHelper::setGeometry(dddb* geo)   {
  deletePtr(m_geometry);
  m_geometry = geo;
}

/// Access visualization attribute for a given volume by path
DD4hep::Geometry::VisAttr DDDBHelper::visAttr(const std::string& path)  const   {
  VisAttrs::const_iterator i = std::find_if(m_visAttrs.begin(), m_visAttrs.end(), ByName(path));
  if ( i == m_visAttrs.end() )  {
    for( i=m_visAttrs.begin(); i != m_visAttrs.end(); ++i)  {
      size_t idx = path.find((*i).first);
      if ( idx == 0 )  {
        return (*i).second;
      }
    }
    return Geometry::VisAttr();
  }
  return (*i).second;
}

/// Add visualization attribute
void DDDBHelper::addVisAttr(const std::string& path, const std::string attr_name)    {
  Geometry::VisAttr attr = m_lcdd.visAttributes(attr_name);
  if ( attr.isValid() )   {
    addVisAttr(path, attr);
  }
}

/// Add visualization attribute
void DDDBHelper::addVisAttr(const std::string& path, DD4hep::Geometry::VisAttr attr)    {
  if ( attr.isValid() )   {
    VisAttrs::const_iterator i = std::find_if(m_visAttrs.begin(), m_visAttrs.end(), ByName(path));
    if ( i == m_visAttrs.end() )  {
      m_visAttrs.push_back(make_pair(path, attr));
    }
  }
}

