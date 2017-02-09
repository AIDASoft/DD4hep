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

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDEve/EveUserContextMenu.h"
#include "DDEve/ElementList.h"

// ROOT include files

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;

ClassImp(ElementList)
ClassImp(ElementListContextMenu)

/// Default constructor
ElementList::ElementList()  : TEveElementList()  {
  InstanceCount::increment(this);
}

/// Initializing constructor
ElementList::ElementList(const std::string& name, const std::string& title, bool doCol, bool doTrans)
  : TEveElementList(name.c_str(), title.c_str(), doCol, doTrans)  {
  InstanceCount::increment(this);
}

/// Copy constructor
ElementList::ElementList(const ElementList& l)
  : TEveElementList(l)  {
  InstanceCount::increment(this);
}

/// Default destructor
ElementList::~ElementList()  {
  InstanceCount::decrement(this);
}

/// Clone object
TEveElementList* ElementList::CloneElement() const  {
  return new ElementList(*this);
}

/// Instantiator
ElementListContextMenu& ElementListContextMenu::install(Display* m)   {
  static ElementListContextMenu s(m);
  return s;
}

/// Initializing constructor
ElementListContextMenu::ElementListContextMenu(Display* mgr)
  : EveUserContextMenu(mgr)
{
  InstallGeometryContextMenu(ElementList::Class());
}

/// Default destructor
ElementListContextMenu::~ElementListContextMenu()  {
}
