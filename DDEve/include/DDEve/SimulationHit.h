// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_SIMULATIONHIT_H
#define DD4HEP_DDEVE_SIMULATIONHIT_H

#ifndef __DD4HEP_DDEVE_EXCLUSIVE__
// Framework include files
#include "DD4hep/Objects.h"
#endif

// C/C++ include files
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  namespace DDEve  {

    class SimulationHit   {
    public:
      typedef Geometry::Position Position;
      Position position;
      float deposit;
      SimulationHit();
      SimulationHit(const Position& p, float d);
      SimulationHit(const SimulationHit& c);
      ~SimulationHit();
      SimulationHit& operator=(const SimulationHit& c);
    };
    typedef std::vector<SimulationHit> SimulationHits;

    inline SimulationHit::SimulationHit() : deposit(0.0) {}
    inline SimulationHit::SimulationHit(const Position& p, float d) : position(p), deposit(d) {}
    inline SimulationHit::SimulationHit(const SimulationHit& c) : position(c.position), deposit(c.deposit) {}
    inline SimulationHit::~SimulationHit()  {}
    inline SimulationHit& SimulationHit::operator=(const SimulationHit& c)  {
      if ( this != &c )  {
	position = c.position;
	deposit = c.deposit;
      }
      return *this;
    }

  }
} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_SIMULATIONHIT_H */

