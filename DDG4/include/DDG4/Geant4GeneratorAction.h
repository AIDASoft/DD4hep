// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4GENERATORACTION_H
#define DD4HEP_DDG4_GEANT4GENERATORACTION_H

// Framework include files
#include "DDG4/Geant4Action.h"

class G4Event;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    /** @class Geant4GeneratorAction Geant4GeneratorAction.h DDG4/Geant4GeneratorAction.h
     * 
     * Concrete implementation of the Geant4 generator action base class
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4GeneratorAction : public Geant4Action    {
    protected:
      Callback m_calls;
    public:
      /// Standard constructor
      Geant4GeneratorAction(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeneratorAction();
      /// Callback to generate primary particles
      virtual void operator()(G4Event* ) {}
    };

    /** @class Geant4GeneratorActionSequence Geant4GeneratorAction.h DDG4/Geant4GeneratorAction.h
     * 
     * Concrete implementation of the Geant4 generator action sequence
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4GeneratorActionSequence : public Geant4Action    {
    protected:
      /// Callback sequence to generate primary particles
      CallbackSequence m_calls;
      /// The list of action objects to be called
      Actors<Geant4GeneratorAction> m_actors;
    public:
      /// Standard constructor
      Geant4GeneratorActionSequence(Geant4Context* context, const std::string& name);
      /// Default destructor
      virtual ~Geant4GeneratorActionSequence();
      /// Register begin-of-run callback. Types Q and T must be polymorph!
      template <typename Q, typename T> 
	void call(Q* p, void (T::*f)(G4Event*))  { m_calls.add(p,f); }
      /// Add an actor responding to all callbacks. Sequence takes ownership.
      void adopt(Geant4GeneratorAction* action);
      /// Callback to generate primary particles
      virtual void operator()(G4Event* event);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4GENERATORACTION_H
