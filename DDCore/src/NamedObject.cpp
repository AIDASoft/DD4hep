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
#include "TObject.h"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

DD4HEP_INSTANTIATE_HANDLE_NAMED(NamedObject);

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
