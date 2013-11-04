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

// Forward declarations
class G4Track;
class G4VTrajectory;
class G4TrackingManager;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  // Forward declarations
  namespace Geometry  {
    class LCDD;
    class DetElement;
  }

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    class Geant4Kernel;
    class Geant4RunActionSequence;
    class Geant4EventActionSequence;
    class Geant4SteppingActionSequence;
    class Geant4TrackingActionSequence;
    class Geant4StackingActionSequence;
    class Geant4GeneratorActionSequence;
    class Geant4SensDetSequences;


    /** @class Geant4Context  Geant4Context.h DDG4/Geant4Context.h
     * 
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4Context    {
      typedef Geometry::LCDD LCDD;

      Geant4Kernel*       m_kernel;
      /// Reference to Geant4 track manager
      G4TrackingManager*  m_trackMgr;
      /// Default constructor
      Geant4Context(Geant4Kernel* kernel);
      /// Default destructor
      virtual ~Geant4Context();
      /// Access the tracking manager
      G4TrackingManager* trackMgr() const  { return m_trackMgr; }
      /// Access to the kernel object
      Geant4Kernel& kernel()               { return *m_kernel;  }
      /// Access to detector description
      LCDD& lcdd() const;
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
