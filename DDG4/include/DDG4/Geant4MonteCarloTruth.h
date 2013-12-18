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

// C/C++ include files

// Forward declarations
class G4Step;
class G4Track;
class G4Event;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4MonteCarloTruth Geant4MonteCarloTruth.h DDG4/Geant4MonteCarloTruth.h
     *
     * Default Interface class to handle monte carlo truth records
     *
     * @author  M.Frank
     * @version 1.0
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
      virtual void mark(const G4Track* track, bool created_hit) = 0;
      /// Mark a Geant4 track of the step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step) = 0;
      /// Store a track produced in a step to be kept for later MC truth analysis
      virtual void mark(const G4Step* step, bool created_hit) = 0;
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4MONTECARLOTRUTH_H
