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

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/InstanceCount.h>

#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4Context.h>
#include <DDG4/Geant4FastSimSpot.h>
#include <DDG4/Geant4HitCollection.h>
#include <DDG4/Geant4SensDetAction.h>

// Geant4 include files
#include <G4Run.hh>
#include <G4Event.hh>
#include <G4Version.hh>
#include <G4TouchableHistory.hh>
#include <G4VSensitiveDetector.hh>
#include <G4VGFlashSensitiveDetector.hh>
#if G4VERSION_NUMBER < 1070
/// Lower versions of Geant4 do not provide G4VFastSimSensitiveDetector
class G4VFastSimSensitiveDetector  {
public:
  G4VFastSimSensitiveDetector() = default;
  virtual ~G4VFastSimSensitiveDetector() = default;
  /// Geant4 Fast simulation interface
  virtual G4bool ProcessHits(const G4FastHit* hit,
			     const G4FastTrack* track,
			     G4TouchableHistory* hist) = 0;
};
#else
#include <G4VFastSimSensitiveDetector.hh>
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Private helper to support sequence reference counting
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename T> class RefCountedSequence  {
    public:
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
        detail::releasePtr(m_sequence);
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
                          virtual public G4VGFlashSensitiveDetector,
                          virtual public G4VFastSimSensitiveDetector,
                          virtual public G4VSDFilter,
                          virtual public Geant4ActionSD,
                          virtual public RefCountedSequence<Geant4SensDetActionSequence>
    {
    protected:
      /// Access to the geant4 sensitive detector handle
      SensitiveDetector m_sensitive;
    public:
      /// Constructor. The detector element is identified by the name
      Geant4SensDet(const std::string& nam, Detector& description)
        : G4VSensitiveDetector(nam),
          G4VGFlashSensitiveDetector(),
          G4VFastSimSensitiveDetector(),
          G4VSDFilter(nam),
          Geant4Action(0,nam),
          Geant4ActionSD(nam),
          RefCountedSequence<Geant4SensDetActionSequence>()
      {
        Geant4Kernel& master = Geant4Kernel::instance(description);
        Geant4Kernel& kernel = master.worker(Geant4Kernel::thread_self());
        m_sensitive   = description.sensitiveDetector(nam);
        m_context     = kernel.workerContext();
        m_outputLevel = kernel.getOutputLevel(nam);
        _aquire(kernel.sensitiveAction(nam));
        m_sequence->defineCollections(this);
        m_sequence->updateContext(m_context);
        this->G4VSensitiveDetector::SetFilter(this);
      }

      /// Destructor
      virtual ~Geant4SensDet() = default;
      /// Overload to avoid ambiguity between G4VSensitiveDetector and G4VSDFilter
      inline G4String GetName() const
      {  return this->G4VSensitiveDetector::SensitiveDetectorName;      }
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string path()  const  override  final
      {  return this->G4VSensitiveDetector::GetPathName();              }
      /// G4VSensitiveDetector internals: Access to the detector path name
      virtual std::string fullPath()  const  override  final
      {  return this->G4VSensitiveDetector::GetFullPathName();          }
      /// Is the detector active?
      virtual bool isActive() const  override
      {  return this->G4VSensitiveDetector::isActive();                 }
      /// This is a utility method which returns the hits collection ID
      virtual G4int GetCollectionID(G4int i)  override  final
      {  return this->G4VSensitiveDetector::GetCollectionID(i);         }
      /// Access to the readout geometry of the sensitive detector
      virtual G4VReadOutGeometry* readoutGeometry() const  override
      {  return this->G4VSensitiveDetector::GetROgeometry();            }
      /// Access to the Detector sensitive detector handle
      virtual SensitiveDetector sensitiveDetector() const  override  final
      {  return m_sensitive;                                            }
      /// Access to the sensitive type of the detector
      virtual const std::string& sensitiveType() const  override  final
      {  return m_sequence->sensitiveType();                            }
      /// Access the DDG4 action sequence
      virtual Geant4SensDetActionSequence* sequence() const  override final
      {  return m_sequence;                                             }
      /// Callback if the sequence should be accepted or filtered off
      virtual G4bool Accept(const G4Step* step) const  override  final
      {  return m_sequence->accept(step);                               }
      /// Method invoked at the beginning of each event.
      virtual void Initialize(G4HCofThisEvent* hce)  override  final
      {  m_sequence->begin(hce);                                        }
      /// Method invoked at the end of each event.
      virtual void EndOfEvent(G4HCofThisEvent* hce)  override  final
      {  m_sequence->end(hce);                                          }
      /// Method for generating hit(s) using the information of G4Step object.
      virtual G4bool ProcessHits(G4Step* step,
                                 G4TouchableHistory* hist)   override  final
      {  return m_sequence->process(step,hist);                         }
      /// GFLASH interface
      virtual G4bool ProcessHits(G4GFlashSpot* sp,
                                 G4TouchableHistory* hist)   override final
      {
        const GFlashEnergySpot* esp = sp->GetEnergySpot();
        G4FastHit         hit(esp->GetPosition(), esp->GetEnergy());
        Geant4FastSimSpot spot(&hit, sp->GetOriginatorTrack(), (sp->GetTouchableHandle())());
        return m_sequence->processFastSim(&spot, hist);
      }
      /// Geant4 Fast simulation interface
      virtual G4bool ProcessHits(const G4FastHit* hit,
                                 const G4FastTrack* track,
                                 G4TouchableHistory* hist)   override final
      {
        Geant4FastSimSpot spot(hit, track, hist);
        return m_sequence->processFastSim(&spot, hist);
      }
      /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
      virtual void clear()  override
      {  m_sequence->clear();                                           }
      /// Initialize the usage of a hit collection. Returns the collection identifier
      virtual size_t defineCollection(const std::string& coll)   override  {
        if ( coll.empty() ) {
          except("Geant4Sensitive: No collection defined for %s [Invalid name]",c_name());
        }
        collectionName.emplace_back(coll);
        return collectionName.size()-1;
      }

    };
  }    // End namespace sim
}      // End namespace dd4hep


#include <DDG4/Factories.h>

typedef dd4hep::sim::Geant4SensDet Geant4SensDet;
typedef dd4hep::sim::Geant4SensDet Geant4tracker;
typedef dd4hep::sim::Geant4SensDet Geant4calorimeter;

DECLARE_GEANT4SENSITIVEDETECTOR(Geant4SensDet)
DECLARE_GEANT4SENSITIVEDETECTOR(Geant4tracker)
DECLARE_GEANT4SENSITIVEDETECTOR(Geant4calorimeter)
