// $Id: Geant4Field.cpp 888 2013-11-14 15:54:56Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
// Framework include files
#ifndef DD4HEP_DDG4_GEANT4MONTECARLORECORDMANAGER_H
#define DD4HEP_DDG4_GEANT4MONTECARLORECORDMANAGER_H

// Framework include files
#include "DDG4/Geant4TrackPersistency.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations

    /** @class Invoke Geant4TrackPersistency.h DDG4/Geant4TrackPersistency.h
     *
     * Default base class for all geant 4 actions and derivates thereof.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4MonteCarloRecordManager : public Geant4Action {
    public:
      /// Flag to indicate if the track information should be collected
      bool m_collectInfo;
    public:
      /// Standard constructor
      Geant4MonteCarloRecordManager(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4MonteCarloRecordManager();
      /// Save G4Track data
      virtual void save(const Geant4TrackPersistency::TrackInfo& track);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4MONTECARLORECORDMANAGER_H
