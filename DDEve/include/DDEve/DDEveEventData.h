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
    /// Hit position
    float x,y,z; 
    /// Energy deposit
    float deposit;
    /// Default constructor
    DDEveHit();
    /// Initializing constructor
    DDEveHit(float xx, float yy, float zz, float d);
    /// Copy constructor
    DDEveHit(const DDEveHit& c);
    /// Default destructor
    ~DDEveHit();
    /// Assignment operator
    DDEveHit& operator=(const DDEveHit& c);
  };
  typedef std::vector<DDEveHit> DDEveHits;

  /// Default constructor
  inline DDEveHit::DDEveHit() : x(0.0), y(0.0), z(0.0), deposit(0.0) {}
  /// Initializing constructor
  inline DDEveHit::DDEveHit(float xx, float yy, float zz, float d) : x(xx), y(yy), z(zz), deposit(d) {}
  /// Copy constructor
  inline DDEveHit::DDEveHit(const DDEveHit& c) : x(c.x), y(c.y), z(c.z), deposit(c.deposit) {}
  /// Default destructor
  inline DDEveHit::~DDEveHit()  {}
  /// Assignment operator
  inline DDEveHit& DDEveHit::operator=(const DDEveHit& c)  {
    if ( this != &c )  {
      x = c.x;
      y = c.y;
      z = c.z;
      deposit = c.deposit;
    }
    return *this;
  }

} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_DDEVEHIT_H */

