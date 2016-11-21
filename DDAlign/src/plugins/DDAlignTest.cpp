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

// Framework includes
#include "DD4hep/Printout.h"
#include "DD4hep/DetectorTools.h"
#include "DD4hep/DetFactoryHelper.h"

using namespace DD4hep;

// ======================================================================================
#include "DDAlign/DDAlignTest.h"
static long install_ddalign_tester(Geometry::LCDD& lcdd, int /* argc */, char** /* argv */)   {
  Alignments::AlignmentsManager alignMgr = Alignments::AlignmentsManager::from(lcdd);
  Conditions::ConditionsManager condMgr  = Conditions::ConditionsManager::from(lcdd);
  DDAlignTest* test = new DDAlignTest();
  test->alignmentsMgr = alignMgr;
  test->conditionsMgr = condMgr;
  lcdd.addExtension<DDAlignTest>(test);
  return 1;
}
DECLARE_APPLY(DDAlign_InstallTest, install_ddalign_tester)
