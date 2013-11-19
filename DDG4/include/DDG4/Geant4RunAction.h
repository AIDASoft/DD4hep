// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4RUNACTION_H
#define DD4HEP_DDG4_GEANT4RUNACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declaration
class G4Run;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {
  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4RunAction Geant4RunAction.h DDG4/Geant4RunAction.h
     *
     * Concrete basic implementation of the Geant4 run action
     *
     * The Run Action is called once per start and end of a run. 
     * i.e. a series of generated events. These two callbacks
     * allow clients to define run-dependent actions such as statistics
     * summaries etc.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4RunAction: public Geant4Action {
    public:
      /// Standard constructor
      Geant4RunAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RunAction();
      /// Begin-of-run callback
      virtual void begin(const G4Run* run);
      /// End-of-run callback
      virtual void end(const G4Run* run);
    };

    /** @class Geant4RunActionSequence Geant4RunAction.h DDG4/Geant4RunAction.h
     *
     * Concrete implementation of the Geant4 run action sequence.
     * The sequence dispatches the callbacks at the beginning and the and
     * of a run to all registered Geant4RunAction members and all 
     * registered callbacks.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4RunActionSequence: public Geant4Action {
    protected:
      /// Callback sequence for begin-run action
      CallbackSequence m_begin;
      /// Callback sequence for end-run action
      CallbackSequence m_end;
      /// The list of action objects to be called
      Actors<Geant4RunAction> m_actors;
    public:
      /// Standard constructor
      Geant4RunActionSequence(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4RunActionSequence();
      /// Register begin-of-run callback. Types Q and T must be polymorph!
      template <typename Q, typename T>
      void callAtBegin(Q* p, void (T::*f)(const G4Run*)) {
        m_begin.add(p, f);
      }
      /// Register end-of-run callback. Types Q and T must be polymorph!
      template <typename Q, typename T>
      void callAtEnd(Q* p, void (T::*f)(const G4Run*)) {
        m_end.add(p, f);
      }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4RunAction* action);
      /// Begin-of-run callback
      virtual void begin(const G4Run* run);
      /// End-of-run callback
      virtual void end(const G4Run* run);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4RUNACTION_H
