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

#ifndef DD4HEP_DDEVE_DDEVEHIT_H
#define DD4HEP_DDEVE_DDEVEHIT_H

// C/C++ include files
#include <vector>
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// DDEve event classes: Basic hit
  /** 
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class DDEveHit   {
  public:
    /// Track/Particle, which produced this hit
    int particle;
    /// Hit position
    float x,y,z; 
    /// Energy deposit
    float deposit;
    /// Default constructor
    DDEveHit();
    /// Initializing constructor
    DDEveHit(int part, float xx, float yy, float zz, float d);
    /// Copy constructor
    DDEveHit(const DDEveHit& c);
    /// Default destructor
    ~DDEveHit();
    /// Assignment operator
    DDEveHit& operator=(const DDEveHit& c);
  };
  typedef std::vector<DDEveHit> DDEveHits;

  /// Data structure to store the MC particle information 
  /**
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class DDEveParticle {
  public:
    int id, parent, pdgID;
    double vsx, vsy, vsz;
    double vex, vey, vez;
    double psx, psy, psz, energy, time;
    std::set<int> daughters;
    /// Default constructor
    DDEveParticle();
    /// Copy constructor
    DDEveParticle(const DDEveParticle& c);
    /// Default destructor
    virtual ~DDEveParticle();
    /// Assignment operator
    DDEveParticle& operator=(const DDEveParticle& c);
  };
  typedef std::vector<DDEveParticle> DDEveParticles;
}      /* End namespace DD4hep     */
#endif /* DD4HEP_DDEVE_DDEVEHIT_H  */

