// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_DDEVEHIT_H
#define DD4HEP_DDEVE_DDEVEHIT_H

// C/C++ include files
#include <vector>
#include <set>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /// DDEve event classes: Basic hit
  /** 
   * @author  M.Frank
   * @version 1.0
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
   * @author  M.Frank
   * @version 1.0
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
} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_DDEVEHIT_H */

