// $Id$
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

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"

#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4SensDetAction.h"

// Geant4 include files
#include "G4VSensitiveDetector.hh"
#include "G4Event.hh"
#include "G4Run.hh"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    Geant4Context* ddg4_globalContext();

    /// Private helper to support sequence reference counting
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename T> class RefCountedSequence  {
    public:
      typedef RefCountedSequence<T> Base;
      T* m_sequence;
      /// Default constructor
      RefCountedSequence() : m_sequence(0) {                 }
      /// Initializing constructor
      RefCountedSequence(T* seq)           {   _aquire(seq); }
      /// Default destructor
      virtual ~RefCountedSequence()        { _release();     }
      void _aquire(T* s)  {
        InstanceCount::increment(this);
        m_sequence = s;
        m_sequence->addRef();
      }
      void _release()  {
        releasePtr(m_sequence);
        InstanceCount::decrement(this);
      }
    };

    /// Concrete implementation of the G4VSensitiveDetector calling the registered sequence object
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4SensDet : virtual public G4VSensitiveDetector,
                          virtual public G4VSDFilter,
                          virtual public Geant4ActionSD,
                          virtual public RefCountedSequence<Geant4SensDetActionSequence>
    {
    protected:
      /// Access to the geant4 sensitive detector handle
      SensitiveDetector m_sensitive;
    public:
      /// Constructor. The detector element is identified by the name
      Geant4SensDet(const std::string& nam, Geometry::LCDD& lcdd)
        : G4VSensitiveDetector(nam), G4VSDFilter(nam),
          Geant4Action(0,nam), Geant4ActionSD(nam), Base()
      {
        Geant4Kernel& kernel = Geant4Kernel::access(lcdd);
        m_sensitive = lcdd.sensitiveDetector(nam);
        m_context = Geant4Context(&kernel);
        m_outputLevel = kernel.getOutputLevel(nam);
        _aquire(kernel.sensitiveAction(nam));
        m_sequence->defineCollections(this);
        this->G4VSensitiveDetector::SetFilter(this);
      }

      /// Destructor
      virtual ~Geant4SensDet()        {                                 }
      /// Overload to avoid ambiguity between G4VSensitiveDetector and G4VSDFilter
      inline G4String GetName() const
      {  return this->G4VSensitiveDetector::SensitiveDetectorName;      }
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string path()  const
      {  return this->G4VSensitiveDetector::GetPathName();              }
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string fullPath()  const
      {  return this->G4VSensitiveDetector::GetFullPathName();          }
      /// Is the detector active?
      virtual bool isActive() const
      {  return this->G4VSensitiveDetector::isActive();                 }
      /// This is a utility method which returns the hits collection ID
      virtual G4int GetCollectionID(G4int i)
      {  return this->G4VSensitiveDetector::GetCollectionID(i);         }
      /// Access to the readout geometry of the sensitive detector
      virtual G4VReadOutGeometry* readoutGeometry() const
      {  return this->G4VSensitiveDetector::GetROgeometry();            }
      /// Access to the LCDD sensitive detector handle
      virtual SensitiveDetector sensitiveDetector() const
      {  return m_sensitive;                                            }
      /// Access to the sensitive type of the detector
      virtual const std::string& sensitiveType() const
      {  return m_sequence->sensitiveType();                            }
      /// Callback if the sequence should be accepted or filtered off
      virtual G4bool Accept(const G4Step* step) const
      {  return m_sequence->accept(step);                               }
      /// Method invoked at the begining of each event.
      virtual void Initialize(G4HCofThisEvent* hce)      {
        m_context.setRun(&ddg4_globalContext()->run());
        m_context.setEvent(&ddg4_globalContext()->event());
        (Geant4Action::ContextUpdate(&m_context))(m_sequence);
        m_sequence->begin(hce);
#if 0
        const G4Event& evt = context()->event();
        error(name(), "%s> calling Initialize(event_id=%d Context: run=%p (%d) evt=%p (%d))",
              GetName().c_str(), evt.GetEventID(),
              &context()->run(), context()->run().run().GetRunID(),
              &context()->event(), context()->event().event().GetEventID());
#endif
      }
      /// Method invoked at the end of each event.
      virtual void EndOfEvent(G4HCofThisEvent* hce)      {
        m_sequence->end(hce);
        m_context.setEvent(0);
        (Geant4Action::ContextUpdate(&m_context))(m_sequence);
      }
      /// Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,G4TouchableHistory* hist)
      {  return m_sequence->process(step,hist);                         }
      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      virtual void clear()
      {  m_sequence->clear();                                           }
      /// Initialize the usage of a hit collection. Returns the collection identifier
      virtual size_t defineCollection(const std::string& coll)  {
        if ( coll.empty() ) {
          except("Geant4Sensitive: No collection defined for %s [Invalid name]",c_name());
        }
        collectionName.push_back(coll);
        return collectionName.size()-1;
      }

    };
  }    // End namespace Simulation
}      // End namespace DD4hep


#include "DDG4/Factories.h"

typedef DD4hep::Simulation::Geant4SensDet Geant4SensDet;
typedef DD4hep::Simulation::Geant4SensDet Geant4tracker;
typedef DD4hep::Simulation::Geant4SensDet Geant4calorimeter;

DECLARE_GEANT4SENSITIVEDETECTOR(Geant4SensDet)
DECLARE_GEANT4SENSITIVEDETECTOR(Geant4tracker)
DECLARE_GEANT4SENSITIVEDETECTOR(Geant4calorimeter)
