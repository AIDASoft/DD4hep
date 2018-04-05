//==============================================================================
//  AIDA Detector description implementation for LHCb
//------------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2018-03-08
//  \version  1.0
//
//==============================================================================
#ifndef DETECTOR_DEALIGNMENTCALLS_H 
#define DETECTOR_DEALIGNMENTCALLS_H 1

// Framework include files
#include "DD4hep/Conditions.h"
#include "DD4hep/ConditionDerived.h"

/// Gaudi namespace declaration
namespace gaudi   {

  /// Callback, which triggers the alignment computation once the delta-parameters are loaded
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeAlignmentCall : public dd4hep::cond::ConditionUpdateCall  {
  public:
    dd4hep::DetElement  top;
    /// Initializing constructor
    DeAlignmentCall(dd4hep::DetElement t) : top(t) {}
    /// Default destructor
    virtual ~DeAlignmentCall() = default;
    /// Interface to client Callback in order to update the condition
    virtual dd4hep::Condition operator()(const dd4hep::ConditionKey& /* key */,
                                         dd4hep::cond::ConditionUpdateContext& ctxt) override final;
  };
  
}      // End namespace gaudi
#endif // DETECTOR_DEALIGNMENTCALLS_H
