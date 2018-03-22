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
#include "Detector/DeVelo.h"
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
  class VeloUpdateContext : public dd4hep::cond::ConditionUpdateUserContext   {
  public:
    std::map<dd4hep::Condition::detkey_type,std::pair<dd4hep::DetElement,dd4hep::DDDB::DDDBCatalog*> > detectors;
    dd4hep::Condition alignments_done;
  };

  /// Base class to share common type definitions
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeConditionCallDefs {
  public:
    typedef dd4hep::DetElement                   DetElement;
    typedef dd4hep::Condition                    Condition;
    typedef dd4hep::ConditionKey                 ConditionKey;
    typedef dd4hep::ConditionKey::KeyMaker       KeyMaker;
    typedef dd4hep::cond::ConditionResolver      Resolver;
    typedef dd4hep::cond::ConditionUpdateContext Context;
    typedef dd4hep::DDDB::DDDBCatalog            Catalog;
  };

  /// Condition derivation call to build the static Velo-pixel DetElement condition information
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloStaticConditionCall : public dd4hep::cond::ConditionUpdateCall, public DeConditionCallDefs  {
  public:
    /// Initializing constructor
    DeVeloStaticConditionCall() = default;
    /// Default destructor
    virtual ~DeVeloStaticConditionCall() = default;
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context) override final;
    /// Interface to client callback for resolving references or to use data from other conditions
    virtual void resolve(Condition c, Resolver& resolver)  override final;
  };

  /// Condition derivation call to build the dynamic Velo-pixel DetElement condition information
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloIOVConditionCall : public dd4hep::cond::ConditionUpdateCall, public DeConditionCallDefs  {
  public:
    DetElement detector;
    Catalog*   catalog = 0;
    VeloUpdateContext* context = 0;    
    /// Initializing constructor
    DeVeloIOVConditionCall(DetElement de, Catalog* cat, VeloUpdateContext* ctx)
      : detector(de), catalog(cat), context(ctx)  {}
    /// Default destructor
    virtual ~DeVeloIOVConditionCall() = default;
    /// Interface to client Callback in order to update the condition
    virtual Condition operator()(const ConditionKey& key, const Context& context) override final;
    /// Interface to client callback for resolving references or to use data from other conditions
    virtual void resolve(Condition c, Resolver& resolver)  override;
  };
  
  /// Condition derivation call to build the dynamic Velo-pixel DetElement condition information
  /**
   *
   *  \author  Markus Frank
   *  \date    2018-03-08
   *  \version  1.0
   */
  class DeVeloConditionCall : public DeVeloIOVConditionCall   {
  public:
    /// Initializing constructor
    DeVeloConditionCall(DetElement de, Catalog* cat, VeloUpdateContext* ctx)
      : DeVeloIOVConditionCall(de,cat,ctx) {}
    /// Default destructor
    virtual ~DeVeloConditionCall() = default;
    /// Interface to client callback for resolving references or to use data from other conditions
    virtual void resolve(Condition c, Resolver& resolver)  override final;
  };
  
}      // End namespace gaudi
#endif // DETECTOR_DEVPCONDITIONCALLS_H
