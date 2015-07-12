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

// Framework include files
#include "DDEve/DisplayConfiguration.h"

// C/C++ include files
#include <stdexcept>

using namespace DD4hep;

ClassImp(DisplayConfiguration)

/// Initializing constructor
DisplayConfiguration::DisplayConfiguration(Display* eve)
: m_display(eve)
{
}

/// Default destructor
DisplayConfiguration::~DisplayConfiguration()  {
}

/// Default constructor
DisplayConfiguration::ViewConfig::ViewConfig() : Config()  {
}

/// Copy constructor
DisplayConfiguration::ViewConfig::ViewConfig(const ViewConfig& c)
  : Config(c), type(c.type),
    subdetectors(c.subdetectors), show_sensitive(c.show_sensitive), show_structure(c.show_structure)
{
}

/// Default destructor
DisplayConfiguration::ViewConfig::~ViewConfig()  {
}

/// Assignment operator
DisplayConfiguration::ViewConfig& DisplayConfiguration::ViewConfig::operator=(const ViewConfig& c)  {
  if ( this == &c ) return *this;
  this->Config::operator=(c);
  show_sensitive = c.show_sensitive;
  show_structure = c.show_structure;
  subdetectors = c.subdetectors;
  type = c.type;
  return *this;
}

/// Default constructor
DisplayConfiguration::Config::Config()   {
  ::memset(&data,0,sizeof(data));
  data.defaults.load_geo = -1;
  data.defaults.show_evt =  1;
  data.defaults.alpha = 0.5;
}

/// Copy constructor
DisplayConfiguration::Config::Config(const Config& c)  {
  name = c.name;
  type = c.type;
  hits = c.hits;
  use  = c.use;
  ::memcpy(&data,&c.data,sizeof(c.data));
}

/// Default destructor
DisplayConfiguration::Config::~Config()  {
}

/// Assignment operator
DisplayConfiguration::Config& DisplayConfiguration::Config::operator=(const Config& c)  {
  if ( this == &c ) return *this;
  name = c.name;
  type = c.type;
  hits = c.hits;
  use  = c.use;
  ::memcpy(&data,&c.data,sizeof(c.data));
  return *this;
}
