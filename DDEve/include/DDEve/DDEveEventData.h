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
#ifndef DDEVE_DDEVEEVENTDATA_H
#define DDEVE_DDEVEEVENTDATA_H

// C/C++ include files
#include <vector>
#include <set>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// DDEve event classes: Basic hit
  /** 
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class DDEveHit   {
  public:
    /// Track/Particle, which produced this hit
    int particle {0};
    /// Hit position
    float x = 0e0, y = 0e0, z = 0e0; 
    /// Energy deposit
    float deposit {0};
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
}      /* End namespace dd4hep     */
#endif // DDEVE_DDEVEEVENTDATA_H

