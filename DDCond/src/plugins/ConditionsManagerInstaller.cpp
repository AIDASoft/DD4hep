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
#include "DD4hep/LCDD.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"
#include "DDCond/ConditionsManager.h"
#include "DDCond/ConditionsInterna.h"

namespace {
  using namespace DD4hep;
  using namespace DD4hep::Conditions;

  int install_cond_mgr (Geometry::LCDD& lcdd, int /* argc */, char** /* argv */)  {
    typedef Interna::ConditionsManagerObject ConditionsManagerObject;
    Handle<ConditionsManagerObject> mgr(lcdd.extension<ConditionsManagerObject>(false));
    if ( !mgr.isValid() )  {
      ConditionsManager mgr_handle(lcdd);
      lcdd.addExtension<ConditionsManagerObject>(mgr_handle.ptr());
      printout(INFO,"ConditionsManager","+++ Successfully installed conditions manager instance.");
    }
    return 1;
  }
}

DECLARE_APPLY(DD4hepConditionsManagerInstaller,install_cond_mgr)
