// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  namespace Geometry {
    class LCDD;
    class DetElement;
  }

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    class Geant4Run;
    class Geant4Event;
    class Geant4Kernel;
    class ContextUpdate;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4GeneratorActionSequence;
    class Geant4SensDetSequences;
    class Geant4MonteCarloTruth;
    class Geant4MonteCarloRecordManager;


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
     *  @author  M.Frank
     *  @version 1.0
     */
    class  Geant4Run : public ObjectExtensions  {
      /// Reference to the original Geant4 run object
      const G4Run* m_run;
    public:
      /// Intializing constructor
      Geant4Run(const G4Run* run);
      /// Default destructor
      virtual ~Geant4Run();
      /// Access the G4Run directly
      operator const G4Run&() const  {  return *m_run;  }
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
      template <typename T> T* addExtension(T* ptr) 	{ 
	return (T*)ObjectExtensions::addExtension(ptr,typeid(T),deletePtr<T>);
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
     *  @author  M.Frank
     *  @version 1.0
     */
    class  Geant4Event : public ObjectExtensions  {
      /// Reference to the original Geant4 event object
      const G4Event* m_event;
    public:
      /// Intializing constructor
      Geant4Event(const G4Event* run);
      /// Default destructor
      virtual ~Geant4Event();
      /// Access the G4Event directly
      operator const G4Event&() const  {  return *m_event;  }
      const G4Event& event() const { return *m_event; }
      /// Add an extension object to the detector element
      /** Note:
       *  To add an extension, which should NOT be deleted,
       *  set 'dtor' to ObjectExtensions::_noDelete or 0.
       */
      void* addExtension(void* ptr, const std::type_info& info, destruct_t dtor)    {
	return ObjectExtensions::addExtension(ptr,info,dtor);
      }
      /// Add user extension object. Ownership is transferred and object deleted at the end of the event.
      template <typename T> T* addExtension(T* ptr) 	{ 
	return (T*)ObjectExtensions::addExtension(ptr,typeid(T),deletePtr<T>);
      }
      /// Access to type safe extension object. Exception is thrown if the object is invalid
      template <typename T> T* extension(bool alert=true) { 
	return (T*)ObjectExtensions::extension(typeid(T),alert); 
      }
    };

    /// Generic context to extend user, run and event information
    /**
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4Context  {
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
    public:
      /// Default constructor
      Geant4Context(Geant4Kernel* kernel);
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
