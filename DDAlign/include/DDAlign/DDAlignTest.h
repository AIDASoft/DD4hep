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
#ifndef DD4HEP_DDALIGNTEST_H
#define DD4HEP_DDALIGNTEST_H

// Framework includes
#include "DDAlign/AlignmentsManager.h"
#include "DDCond/ConditionsManager.h"


/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Test class to chain variou test sequences for DDCond and DDAlign
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP
   */
  class DDAlignTest  {
    typedef Alignments::AlignmentsManager AlignmentsManager;
    typedef Alignments::UserPool          AlignmentsPool;

    typedef Conditions::ConditionsManager ConditionsManager;
    typedef Conditions::UserPool          ConditionsPool;

  public:
    /// Reference to alignments manager object
    AlignmentsManager          alignmentsMgr;
    dd4hep_ptr<ConditionsPool> alignmentsPool;

    /// Reference to conditions manager object
    ConditionsManager          conditionsMgr;
    dd4hep_ptr<ConditionsPool> conditionsPool;

  public:
    /// Default constructor
    DDAlignTest() = default;
    /// Default destructor
    virtual ~DDAlignTest() = default;
  };
}      /* End namespace DD4hep         */
#endif /* DD4HEP_DDALIGN_DDALIGNTEST_H */
