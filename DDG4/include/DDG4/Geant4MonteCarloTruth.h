// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H
#define DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H

// Framework include files
#include "DDG4/Geant4Action.h"

// C/C++ include files
#include <map>

// Forward declarations
class G4Step;
class G4Track;
class G4Event;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4Particle;

    /// Default Interface class to handle monte carlo truth records
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4MonteCarloTruth   {
    protected:
      /// Standard constructor
      Geant4MonteCarloTruth();
    public:
      /// Default destructor
      virtual ~Geant4MonteCarloTruth();
      /// Mark a Geant4 track to be kept for later MC truth analysis
      virtual void mark(const G4Track* track) = 0;
      /// Store a track, with a flag
      virtual void mark(const G4Track* track, int reason) = 0;
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step) = 0;
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, int reason) = 0;
    };

    /// Void implementation of the Monte-Carlo thruth handler doing nothing at all.
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4DummyTruthHandler : public Geant4Action, public Geant4MonteCarloTruth  {
    public:
      /// Standard constructor
      Geant4DummyTruthHandler(Geant4Context* ctxt,const std::string& nam);
      /// Default destructor
      virtual ~Geant4DummyTruthHandler();
      /// Mark a Geant4 track to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Track* track);
      /// Store a track, with a flag
      virtual void mark(const G4Track* track, int reason);
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis. Default flag: CREATED_HIT
      virtual void mark(const G4Step* step);
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, int reason);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H
