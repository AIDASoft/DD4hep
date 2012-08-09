// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_GEANT4SENSITIVEDETECTOR_H
#define DD4HEP_GEANT4SENSITIVEDETECTOR_H

// Framework include files
#include "DD4hep/LCDD.h"
#include "DDG4/Geant4Hits.h"

// Geant 4 include files
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"

#include <algorithm>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation {

    /** @class Geant4SensitiveDetector Geant4SensitiveDetector.h DDG4/Geant4SensitiveDetector.h
     *
     * @author  M.Frank
     * @version 1.0
     */
    struct Geant4SensitiveDetector : public G4VSensitiveDetector  {
      public:      

      typedef std::vector<std::string>     HitCollectionNames;
      typedef G4THitsCollection<Geant4Hit> HitCollection;
      typedef Geant4Hit::Contribution      HitContribution;

      protected:

      /// Reference to the detector description object
      LCDD&             m_lcdd;
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

      public:

      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4SensitiveDetector(const std::string& name, LCDD& lcdd);

      /// Initialize the sensitive detector for the usage of a single hit collection
      virtual bool defineCollections(const std::string& coll_name);

      /// Initialize the sensitive detector for the usage of a multiple hit collection
      virtual bool defineCollections(const HitCollectionNames& coll_names);

      /// Standard destructor
      virtual ~Geant4SensitiveDetector();

      /// Standard access to the name
      const std::string& name() const {  return GetName(); }

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

      /** G4VSensitiveDetector interface: Method invoked at the begining of each event. 
       *  The hits collection(s) created by this sensitive detector must
       *  be set to the G4HCofThisEvent object at one of these two methods.
       */
      virtual void Initialize(G4HCofThisEvent* HCE);

      /// G4VSensitiveDetector interface: Method invoked at the end of each event. 
      virtual void EndOfEvent(G4HCofThisEvent* HCE);

      /// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* history);

      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      /** Hits collections created but not beibg set to G4HCofThisEvent 
       *  at the event should be deleted.
       *  Collection(s) which have already set to G4HCofThisEvent 
       *  will be deleted automatically.
       */
      virtual void clear();

    };


    template <class T> class Geant4GenericSD : public Geant4SensitiveDetector {
      T   userData;
    public:
      /// Constructor. The sensitive detector element is identified by the detector name
      Geant4GenericSD(const std::string& name, LCDD& lcdd);
      
      /// Initialize the sensitive detector for the usage of a single hit collection
      virtual bool defineCollections(const std::string& coll_name);

      /// Initialize the sensitive detector for the usage of a multiple hit collection
      virtual bool defineCollections(const HitCollectionNames& coll_names);

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

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif // DD4HEP_GEANT4SENSITIVEDETECTOR_H
