// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKINGPREACTION_H
#define DD4HEP_DDG4_GEANT4TRACKINGPREACTION_H

// Framework include files
#include "DDG4/Geant4TrackingAction.h"

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4TrackingPreAction Geant4TrackingPreAction.h DDG4/Geant4TrackingPreAction.h
     *
     * Default base class for all geant 4 tracking actions.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackingPreAction: public Geant4TrackingAction {
    public:
      /// Standard constructor
      Geant4TrackingPreAction(Geant4Context* context, const std::string& name = "");
      /// Default destructor
      virtual ~Geant4TrackingPreAction();
      /// Begin-of-tracking callback
      virtual void begin(const G4Track* track);
      /// End-of-tracking callback
      virtual void end(const G4Track* track);
    };
  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINGPREACTION_H
