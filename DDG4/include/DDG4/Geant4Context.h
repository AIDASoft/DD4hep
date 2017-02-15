//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#ifndef DD4HEP_DDG4_GEANT4CONTEXT_H
#define DD4HEP_DDG4_GEANT4CONTEXT_H

// Framework incloude files
#include "DD4hep/Primitives.h"
#include "DD4hep/ObjectExtensions.h"

// Forward declarations
class G4Run;
class G4Track;
class G4Event;
class G4VTrajectory;
class G4TrackingManager;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
    class DetElement;
  }

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    class Geant4Run;
    class Geant4Event;
    class Geant4Kernel;
    class Geant4Random;
    class ContextUpdate;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4GeneratorActionSequence;
    class Geant4SensDetSequences;

    ///  User run context for DDG4
    /**
     *  The context is accessible from the Geant4Context pointer,
     *  which is present in every Geant4Action.
     *  The run context is only valid however, if there is actually
     *  an run being procesed ie. only during the lifetime of the
     *  corresponding G4Run objects.
     *
     *  Please note:
     *  Extensions are only valid within the hosting run and are INVALID
     *  across different runs.
     *  Hence: They are only useful to extend data of an run.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class  Geant4Run : public ObjectExtensions  {
      /// Reference to the original Geant4 run object
      const G4Run* m_run;
    protected:

    public:
      /// Intializing constructor
      Geant4Run(const G4Run* run);
      /// Default destructor
      virtual ~Geant4Run();
      /// Access the G4Run directly: Automatic type conversion
      operator const G4Run&() const  {  return *m_run;  }
      /// Access the G4Event directly: Explicit G4Run accessor
      const G4Run& run() const       { return *m_run;   }
      /// Add an extension object to the detector element
      /** Note:
       *  To add an extension, which should NOT be deleted,
       *  set 'dtor' to ObjectExtensions::_noDelete or 0.
       */
      void* addExtension(void* ptr, const std::type_info& info, destruct_t dtor)    {
        return ObjectExtensions::addExtension(ptr,info,dtor);
      }
      /// Add user extension object. Ownership is transferred!
      template <typename T> T* addExtension(T* ptr, bool take_ownership=true)   {
        destruct_t dt = ObjectExtensions::_delete<T>;
        return (T*)ObjectExtensions::addExtension(ptr,typeid(T),take_ownership ? dt : 0);
      }
      /// Access to type safe extension object. Exception is thrown if the object is invalid
      template <typename T> T* extension(bool alert=true) {
        return (T*)ObjectExtensions::extension(typeid(T),alert);
      }
    };

    ///  User event context for DDG4
    /**
     *  The context is accessible from the Geant4Context pointer,
     *  which is present in every Geant4Action.
     *  The event context is only valid however, if there is actually
     *  an event being procesed ie. only during the lifetime of the
     *  corresponding G4Event objects.
     *
     *  Please note:
     *  Extensions are only valid within the hosting event and are INVALID
     *  across different events.
     *  Hence: They are only useful to extend data of an event.
     *
     *  Any random numbers used to process one event should be accessed
     *  from this location. The framework ensures that the same seeded
     *  sequence is used throughout the processing of one single event.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class  Geant4Event : public ObjectExtensions  {
      /// Reference to the original Geant4 event object
      const G4Event* m_event;
      /// Reference to the main random number generator
      Geant4Random* m_random;

    public:
      /// Intializing constructor
      Geant4Event(const G4Event* run, Geant4Random* rndm);
      /// Default destructor
      virtual ~Geant4Event();
      /// Access the G4Event directly: Automatic type conversion
      operator const G4Event&() const  {  return *m_event;   }
      /// Access the G4Event directly: Explicit G4Event accessor
      const G4Event& event() const     {  return *m_event;   }
      /// Access the random number generator
      Geant4Random& random() const     {  return *m_random;  }

      /// Add an extension object to the detector element
      /** Note:
       *  To add an extension, which should NOT be deleted,
       *  set 'dtor' to ObjectExtensions::_noDelete or 0.
       */
      void* addExtension(void* ptr, const std::type_info& info, destruct_t dtor)    {
        return ObjectExtensions::addExtension(ptr,info,dtor);
      }
      /// Add user extension object. Ownership is transferred and object deleted at the end of the event.
      template <typename T> T* addExtension(T* ptr, bool take_ownership=true)   {
        destruct_t dt = ObjectExtensions::_delete<T>;
        return (T*)ObjectExtensions::addExtension(ptr,typeid(T),take_ownership ? dt : 0);
      }
      /// Access to type safe extension object. Exception is thrown if the object is invalid
      template <typename T> T* extension(bool alert=true) {
        return (T*)ObjectExtensions::extension(typeid(T),alert);
      }
    };

    /// Generic context to extend user, run and event information
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Context  {
      friend class Geant4Kernel;
    public:
#ifdef R__DICTIONARY_FILENAME
      /// ROOT does not know how to process the nested ns otherwise
    public:
      typedef Geometry::LCDD LCDD;
#endif
    protected:
      Geant4Kernel* m_kernel;
      Geant4Run*    m_run;
      Geant4Event*  m_event;
      /// Default constructor
      Geant4Context(Geant4Kernel* kernel);
    public:
      /// Default destructor
      virtual ~Geant4Context();
      /// Set the geant4 run reference
      void setRun(Geant4Run* new_run);
      /// Access the geant4 run -- valid only between BeginRun() and EndRun()!
      Geant4Run& run()  const;
      /// Access the geant4 run by ptr. Must be checked by clients!
      Geant4Run* runPtr()  const  { return m_run; }
      /// Set the geant4 event reference
      void setEvent(Geant4Event* new_event);
      /// Access the geant4 event -- valid only between BeginEvent() and EndEvent()!
      Geant4Event& event()  const;
      /// Access the geant4 event by ptr. Must be checked by clients!
      Geant4Event* eventPtr()  const  { return m_event; }
      /// Access to the kernel object
      Geant4Kernel& kernel()  const   { return *m_kernel;   }
      /// Access to detector description
      Geometry::LCDD& lcdd() const;
      /// Access the tracking manager
      G4TrackingManager* trackMgr() const;
      /// Create a user trajectory
      virtual G4VTrajectory* createTrajectory(const G4Track* track) const;
      /// Access to the main run action sequence from the kernel object
      Geant4RunActionSequence& runAction() const;
      /// Access to the main event action sequence from the kernel object
      Geant4EventActionSequence& eventAction() const;
      /// Access to the main stepping action sequence from the kernel object
      Geant4SteppingActionSequence& steppingAction() const;
      /// Access to the main tracking action sequence from the kernel object
      Geant4TrackingActionSequence& trackingAction() const;
      /// Access to the main stacking action sequence from the kernel object
      Geant4StackingActionSequence& stackingAction() const;
      /// Access to the main generator action sequence from the kernel object
      Geant4GeneratorActionSequence& generatorAction() const;
      /// Access to the sensitive detector sequences from the kernel object
      Geant4SensDetSequences& sensitiveActions() const;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_DDG4_GEANT4CONTEXT_H
