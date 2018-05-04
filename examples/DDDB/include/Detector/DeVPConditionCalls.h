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
#ifndef DETECTOR_DEVPCONDITIONCALLS_H 
#define DETECTOR_DEVPCONDITIONCALLS_H 1

// Framework include files
#include "Detector/DeVP.h"
#include "Detector/DeConditionCallDefs.h"
#include "DD4hep/ConditionDerived.h"

/// Gaudi namespace declaration
namespace gaudi   {

  /// Context information used when computing the Velo-pixel derived conditions
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class VPUpdateContext : public dd4hep::cond::ConditionUpdateUserContext   {
  public:
    std::map<dd4hep::Condition::detkey_type,std::pair<dd4hep::DetElement,dd4hep::DDDB::DDDBCatalog*> > detectors;
    dd4hep::Condition alignments_done;
  };

  /// Condition derivation call to build the static Velo-pixel DetElement condition information
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPStaticConditionCall : public dd4hep::cond::ConditionUpdateCall, public DeConditionCallDefs  {
  public:
    /// Initializing constructor
    DeVPStaticConditionCall() = default;
    /// Default destructor
    virtual ~DeVPStaticConditionCall() = default;
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, Context& context) override final;
    /// Interface to client callback for resolving references or to use data from other conditions
    virtual void resolve(Condition c, Context& context)  override final;
  };

  /// Condition derivation call to build the dynamic Velo-pixel DetElement condition information
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPIOVConditionCall : public dd4hep::cond::ConditionUpdateCall, public DeConditionCallDefs  {
  public:
    DetElement detector;
    Catalog*   catalog = 0;
    VPUpdateContext* velo_context = 0;    
    /// Initializing constructor
    DeVPIOVConditionCall(DetElement de, Catalog* cat, VPUpdateContext* ctx)
      : detector(de), catalog(cat), velo_context(ctx)  {}
    /// Default destructor
    virtual ~DeVPIOVConditionCall() = default;
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, Context& context) override final;
    /// Interface to client callback for resolving references or to use data from other conditions
    virtual void resolve(Condition c, Context& context)  override;
  };
  
  /// Condition derivation call to build the dynamic Velo-pixel DetElement condition information
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVPConditionCall : public DeVPIOVConditionCall   {
  public:
    /// Initializing constructor
    DeVPConditionCall(DetElement de, Catalog* cat, VPUpdateContext* ctx)
      : DeVPIOVConditionCall(de,cat,ctx) {}
    /// Default destructor
    virtual ~DeVPConditionCall() = default;
    /// Interface to client callback for resolving references or to use data from other conditions
    virtual void resolve(Condition c, Context& context)  override final;
  };
  
}      // End namespace gaudi
#endif // DETECTOR_DEVPCONDITIONCALLS_H
