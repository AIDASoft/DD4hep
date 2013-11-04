// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4STEPPINGACTION_H
#define DD4HEP_DDG4_GEANT4STEPPINGACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

// Forward declarations
class G4SteppingManager;
class G4Step;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    /** @class Geant4SteppingAction Geant4Action.h DDG4/Geant4Action.h
     * 
     * Concrete implementation of the Geant4 stepping action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4SteppingAction : public Geant4Action   {
    public:
      /// Standard constructor
      Geant4SteppingAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4SteppingAction();
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
    };

    /** @class Geant4SteppingActionSequence Geant4Action.h DDG4/Geant4Action.h
     * 
     * Concrete implementation of the Geant4 stepping action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4SteppingActionSequence : public Geant4Action   {
    protected:
      /// Callback sequence for user stepping action calls
      CallbackSequence m_calls;
      /// The list of action objects to be called
      Actors<Geant4SteppingAction> m_actors;

    public:
      /// Standard constructor
      Geant4SteppingActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4SteppingActionSequence();
      /// Register stepping action callback. Types Q and T must be polymorph!
      template <typename Q, typename T> 
	void call(Q* p, void (T::*f)(const G4Step*,G4SteppingManager*))  { m_calls.add(p,f); }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4SteppingAction* action);
      /// User stepping callback
      virtual void operator()(const G4Step* step, G4SteppingManager* mgr);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4STEPPINGACTION_H
