//==========================================================================
//  AIDA Detector description implementation 
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
#ifndef DDG4_GEANT4SENSITIVEDETECTOR_H
#define DDG4_GEANT4SENSITIVEDETECTOR_H

// Framework include files
#include "DD4hep/Detector.h"
#include "DDG4/Geant4Hits.h"

// Geant 4 include files
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"

#include <algorithm>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    // Forward declarations
    class Geant4StepHandler;
    class Geant4SensitiveDetector;

    /// Deprecated: Example G4VSensitiveDetector implementation.
    /**  \deprecated
     * Deprecated class. Not supported by the DDG4 kernel.
     *
     * @author  M.Frank
     * @version 1.0
     */
    class Geant4SensitiveDetector : public G4VSensitiveDetector  {
    public:
      typedef G4THitsCollection<Geant4Hit> HitCollection;
      typedef Geant4Hit::Contribution      HitContribution;
      typedef Geant4StepHandler            StepHandler;

    protected:

      /// Reference to the detector description object
      Detector&             m_detDesc;
      /// Reference to the detector element describing this sensitive element
      DetElement        m_detector;
      /// Reference to the sensitive detector element
      SensitiveDetector m_sensitive;
      /// Reference to the readout structure
      Readout           m_readout;

      /// Geant4 event context
      G4HCofThisEvent*  m_hce;

      /// Find hits by position in a collection
      template <typename T> T* find(const HitCollection* c,const HitCompare<T>& cmp);

      /// Dump Step information (careful: very verbose)
      void dumpStep(G4Step* step,G4TouchableHistory* history);

    public:

      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4SensitiveDetector(const std::string& name, Detector& description);

      /// Initialize the sensitive detector for the usage of a single hit collection
      virtual bool defineCollection(const std::string& coll_name);

      /// Standard destructor
      virtual ~Geant4SensitiveDetector();

      /// Standard access to the name
      std::string name() const {  return GetName(); }

      /// Create single hits collection
      virtual HitCollection* createCollection(const std::string& coll_name)  const;

      /// Access HitCollection container names
      const std::string& hitCollectionName(int which) const;

      /// Retrieve the hits collection associated with this detector by its serial number
      HitCollection* collection(int id);

      /// Retrieve the hits collection associated with this detector by its collection identifier
      HitCollection* collectionByID(int id);

      /// Method for generating hit(s) using the information of G4Step object.
      virtual bool buildHits(G4Step* /* step */,G4TouchableHistory* /* history */) { return false; }


      /// Returns the volumeID of the sensitive volume corresponding to the step -
      /// combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
      //  from the current sensitive volume to the world volume
      long long getVolumeID(G4Step* step);

      /// Returns the volumeID of the sensitive volume corresponding to the step -
      /// combining the VolIDS of the complete geometry path (Geant4TouchableHistory)
      //  from the current sensitive volume to the world volume
      long long getCellID(G4Step* step);

      /** G4VSensitiveDetector interface: Method invoked at the begining of each event.
       *  The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void Initialize(G4HCofThisEvent* HCE);

      /// G4VSensitiveDetector interface: Method invoked at the end of each event.
      virtual void EndOfEvent(G4HCofThisEvent* HCE);

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* history);

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual G4bool process(G4Step* step,G4TouchableHistory* history);

      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      /** Hits collections created but not beibg set to G4HCofThisEvent
       *  at the event should be deleted.
       *  Collection(s) which have already set to G4HCofThisEvent
       *  will be deleted automatically.
       */
      virtual void clear();
    };


    /// Deprecated: Example G4VSensitiveDetector implementation.
    /**  \deprecated
     * Deprecated class. Not supported by the DDG4 kernel.
     */
    template <class T> class Geant4GenericSD : public Geant4SensitiveDetector {
      T   userData;
    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4GenericSD(const std::string& name, Detector& description);

      /// Initialize the sensitive detector for the usage of a single hit collection
      virtual bool defineCollection(const std::string& coll_name);

      /// Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* history);

      /** Method invoked at the begining of each event.
       *  The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void Initialize(G4HCofThisEvent* HCE);

      /// Method invoked at the end of each event.
      virtual void EndOfEvent(G4HCofThisEvent* HCE);

      ///  This method is invoked if the event abortion is occured.
      /** Hits collections created but not beibg set to G4HCofThisEvent
       *  at the event should be deleted.
       *  Collection(s) which have already set to G4HCofThisEvent
       *  will be deleted automatically.
       */
      virtual void clear();

      /// Method for generating hit(s) using the information of G4Step object.
      virtual bool buildHits(G4Step* step,G4TouchableHistory* history);
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4SENSITIVEDETECTOR_H
