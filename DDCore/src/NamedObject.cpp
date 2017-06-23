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

// Framework includes
#include "DD4hep/NamedObject.h"
#include "DD4hep/detail/Handle.inl"
#include "TNamed.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

namespace dd4hep {
  template <> const char* Handle<NamedObject>::name() const  {
    return this->m_element ? this->m_element->name.c_str() : "";
  }
  template <> void
  Handle<NamedObject>::assign(NamedObject* p, const string& n, const string& t){
    m_element = p;
    p->name = n;
    p->type = t;
  }
}
template class dd4hep::Handle<NamedObject>;

/// Initializing constructor
NamedObject::NamedObject(const char* nam, const char* typ)
  : name(nam ? nam : ""), type(typ ? typ : "")
{
}

/// Initializing constructor
NamedObject::NamedObject(const std::string& nam)
  : name(nam), type()
{
}

/// Initializing constructor
NamedObject::NamedObject(const std::string& nam, const std::string& typ)
  : name(nam), type(typ)
{
}
