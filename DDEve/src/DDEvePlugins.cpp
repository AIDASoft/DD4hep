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


// Framework include files
#include "DD4hep/detail/Plugins.inl"
#include "DDEve/Factories.h"

DD4HEP_IMPLEMENT_PLUGIN_REGISTRY(dd4hep::View*, (dd4hep::Display*, const char*))
