// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4TRACKINGACTION_H
#define DD4HEP_DDG4_GEANT4TRACKINGACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"
#include "G4VUserTrackInformation.hh"

class G4TrackingManager;
class G4Track;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    // Forward declarations
    class Geant4TrackInformation;

    /** @class Geant4TrackingAction Geant4Action.h DDG4/Geant4Action.h
     *
     * Default base class for all geant 4 tracking actions.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackingAction: public Geant4Action {
    public:
      /// Standard constructor
      Geant4TrackingAction(Geant4Context* context, const std::string& name = "");
      /// Default destructor
      virtual ~Geant4TrackingAction();
      /// Access the tracking manager
      G4TrackingManager* trackMgr() const {
        return m_context->trackMgr();
      }
      /// Get the valid Geant4 tarck information
      Geant4TrackInformation* trackInfo(G4Track* track) const;
      /// Mark all children of the track to be stored
      bool storeChildren() const;
      /// Mark a single child of the track to be stored
      bool storeChild(Geant4TrackInformation* info) const;
      /// Pre-track action callback
      virtual void begin(const G4Track* trk);
      /// Post-track action callback
      virtual void end(const G4Track* trk);
    };

    /** @class Geant4EventActionSequence Geant4Action.h DDG4/Geant4Action.h
     *
     * Concrete implementation of the Geant4 tracking action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4TrackingActionSequence: public Geant4Action {
    protected:
      /// Callback sequence for pre tracking action
      CallbackSequence m_begin;
      /// Callback sequence for post tracking action
      CallbackSequence m_end;
      /// The list of action objects to be called
      Actors<Geant4TrackingAction> m_actors;
    public:
      /// Standard constructor
      Geant4TrackingActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4TrackingActionSequence();
      /// Register Pre-track action callback
      template <typename Q, typename T> void callAtBegin(Q* p, void (T::*f)(const G4Track*)) {
        m_begin.add(p, f);
      }
      /// Register Post-track action callback
      template <typename Q, typename T> void callAtEnd(Q* p, void (T::*f)(const G4Track*)) {
        m_end.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4TrackingAction* action);
      /// Pre-track action callback
      virtual void begin(const G4Track* trk);
      /// Post-track action callback
      virtual void end(const G4Track* trk);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4TRACKINGACTION_H
