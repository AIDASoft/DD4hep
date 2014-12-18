#include "DDG4/Geant4Data.h"

using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the DD4hep event display specializations
  namespace DDEve  {

    /// SimulationHit definition
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     *  \ingroup DD4HEP_EVE
     */
    class SimulationHit   {
    public:
      Position position;
      float deposit;
      /// Default constructor
      SimulationHit() : deposit(0.0) {}
      /// Standard initializing constructor
      SimulationHit(const Position& p, float d) : position(p), deposit(d) {}
      /// Copy constructor
      SimulationHit(const SimulationHit& c) : position(c.position), deposit(c.deposit) {}
      /// Standard Destructor
      ~SimulationHit()  {}
      /// Assignment operator
      SimulationHit& operator=(const SimulationHit& c)  {
        if ( this != &c )  {
          position = c.position;
          deposit = c.deposit;
        }
        return *this;
      }
    };
  }
}

/// Hit conversion function  \ingroup DD4HEP_EVE
static void* _convertHitCollection(const char* source)  {
  typedef DD4hep::DDEve::SimulationHit SimulationHit;
  const std::vector<SimpleHit*>* c = (std::vector<SimpleHit*>*)source;
  std::vector<SimulationHit>* pv = new std::vector<SimulationHit>();
  if ( source && c->size() > 0 )   {
    for(std::vector<SimpleHit*>::const_iterator k=c->begin(); k!=c->end(); ++k)   {
      SimpleTracker::Hit* trh = dynamic_cast<SimpleTracker::Hit*>(*k);
      if ( trh )   {
        pv->push_back(SimulationHit(trh->position, trh->energyDeposit));
        continue;
      }
      SimpleCalorimeter::Hit* cah = dynamic_cast<SimpleCalorimeter::Hit*>(*k);
      if ( cah )   {
        pv->push_back(SimulationHit(cah->position, cah->energyDeposit));
        continue;
      }
    }
  }
  return pv;
}

#include "DD4hep/Factories.h"
using namespace DD4hep::Geometry;
DECLARE_CONSTRUCTOR(DDEve_DDG4CollectionAccess,_convertHitCollection)
