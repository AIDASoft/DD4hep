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
#ifndef DD4HEP_DDEVE_HITHANDLERS_H
#define DD4HEP_DDEVE_HITHANDLERS_H

// Framework include files
#include "DDEve/EventHandler.h"
#include "DDEve/DisplayConfiguration.h"

// Forward declarations
class THF2;
class TEveElement;
class TEvePointSet;
class TEveBoxSet;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Fill EtaPhi histograms from a hit collection
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct EtaPhiHistogramActor : public DDEveHitActor  {
    TH2F* histogram;
    /// Standard initializing constructor
    EtaPhiHistogramActor(TH2F* h) : DDEveHitActor(), histogram(h) {}
    /// Action callback of this functor: 
    virtual void operator()(const DDEveHit& hit);
  };

  /// Fill a 3D pointset from a hit collection.
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct PointsetCreator : public DDEveHitActor  {
    TEvePointSet* pointset;
    float deposit;
    int count;
    /// Standard initializing constructor
    PointsetCreator(const std::string& collection, size_t length);
    /// Standard initializing constructor
    PointsetCreator(const std::string& collection, size_t length, const DisplayConfiguration::Config& cfg);
    /// Standard destructor
    virtual ~PointsetCreator();
    /// Return eve element
    TEveElement* element() const;
    /// Action callback of this functor: 
    virtual void operator()(const DDEveHit& hit);
  };

  /// Fill a 3D box set from a hit collection.
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct BoxsetCreator : public DDEveHitActor  {
    TEveBoxSet* boxset;
    float emax, towerH, deposit;
    int count;
    /// Standard initializing constructor
    BoxsetCreator(const std::string& collection, size_t length);
    /// Standard initializing constructor
    BoxsetCreator(const std::string& collection, size_t length, const DisplayConfiguration::Config& cfg);
    /// Standard destructor
    virtual ~BoxsetCreator();
    /// Return eve element
    TEveElement* element() const;
    /// Action callback of this functor: 
    virtual void operator()(const DDEveHit& hit);
  };

  /// Fill a 3D tower set from a hit collection.
  /*
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  struct TowersetCreator : public BoxsetCreator  {
    /// Standard initializing constructor
    TowersetCreator(const std::string& collection, size_t length) 
      : BoxsetCreator(collection,length) {}
    /// Standard initializing constructor
    TowersetCreator(const std::string& collection, size_t length, const DisplayConfiguration::Config& cfg)
      : BoxsetCreator(collection, length, cfg) {}
    /// Standard destructor
    virtual ~TowersetCreator() {}
    /// Action callback of this functor: 
    virtual void operator()(const DDEveHit& hit);
  };

} /* End namespace dd4hep   */


#endif /* DD4HEP_DDEVE_HITHANDLERS_H */

