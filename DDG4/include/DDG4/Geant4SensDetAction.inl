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

/// Framework include files
#include <DDG4/Geant4SensDetAction.h>
#include <DD4hep/Detector.h>
#include <DD4hep/InstanceCount.h>
#include <DD4hep/detail/ObjectsInterna.h>

#include <DDG4/Geant4ReadoutVolumeFilter.h>
#include <DDG4/Geant4Data.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    /// Standard , initializing constructor
    template <typename T>
    Geant4SensitiveAction<T>::Geant4SensitiveAction(Geant4Context* ctxt,
						    const std::string& nam,
						    DetElement det,
						    Detector& description_ref)
      : Geant4Sensitive(ctxt,nam,det,description_ref), m_collectionName(), m_collectionID(0)
    {
      declareProperty("ReadoutName",    m_readoutName);
      declareProperty("CollectionName", m_collectionName);
      initialize();
      InstanceCount::increment(this);
    }

    /// Default destructor
    template <typename T> Geant4SensitiveAction<T>::~Geant4SensitiveAction()  {
      finalize();
      InstanceCount::decrement(this);
    }

    /// Initialization overload for specialization
    template <typename T> void Geant4SensitiveAction<T>::initialize()   {
    }

    /// Finalization overload for specialization
    template <typename T> void Geant4SensitiveAction<T>::finalize()   {
    }

    /// Access the readout object. Note: if m_readoutName is set, the m_readout != m_sensitive.readout()
    template <typename T> Readout Geant4SensitiveAction<T>::readout()    {
      if ( !m_readoutName.empty() && m_sensitive.readout() == m_readout )  {
        m_readout = detectorDescription().readout(m_readoutName);
        if ( !m_readout.isValid() )  {
          except("+++ Failed to access parallel readout '%s'",m_sensitive.name());
        }
        m_segmentation = m_readout.segmentation();
        if ( !m_segmentation.isValid() )  {
          except("+++ Failed to access segmentation for readout '%s'",m_readout.name());
        }
      }
      return m_readout;
    }

    /// G4VSensitiveDetector interface: Method invoked at the beginning of each event.
    template <typename T> void Geant4SensitiveAction<T>::begin(G4HCofThisEvent* hce)  {
      Geant4Sensitive::begin(hce);
    }

    /// G4VSensitiveDetector interface: Method invoked at the end of each event.
    template <typename T> void Geant4SensitiveAction<T>::end(G4HCofThisEvent* hce)  {
      Geant4Sensitive::end(hce);
    }

    /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
    template <typename T> void Geant4SensitiveAction<T>::clear(G4HCofThisEvent* hce)  {
      Geant4Sensitive::clear(hce);
    }

    /// Define collections created by this sensitivie action object
    template <typename T> void Geant4SensitiveAction<T>::defineCollections()  {
    }

    /// Define readout specific hit collection. matching name must be present in readout structure
    template <typename T> template <typename HIT> 
    std::size_t Geant4SensitiveAction<T>::defineReadoutCollection(const std::string match_name)  {
      auto ro = readout();
      for(const auto& c : ro->hits )  {
        if ( c.name == match_name )  {
          std::size_t coll_id = defineCollection<HIT>(match_name);
          Geant4Filter* filter = new Geant4ReadoutVolumeFilter(context(),name()+"_"+c.name,ro,c.name);
          adopt_front(filter);
          return coll_id;
        }
      }
      except("+++ Custom collection name '%s' not defined in the Readout object: %s.",
             m_collectionName.c_str(), ro.name());
      return 0; // Anyhow not reachable. Exception thrown before
    }

    /// Define readout specific hit collection with volume ID filtering
    template <typename T> template <typename HIT> 
    std::size_t Geant4SensitiveAction<T>::declareReadoutFilteredCollection()   {
      if ( m_collectionName.empty() )  {
        return defineCollection<HIT>(readout().name());
      }
      return defineReadoutCollection<HIT>(m_collectionName);
    }

    /// G4VSensitiveDetector interface: Method for generating hit(s) using the G4Step object.
    template <typename T> bool Geant4SensitiveAction<T>::process(const G4Step* step,G4TouchableHistory* history)  {
      return Geant4Sensitive::process(step, history);
    }
    
    /// GFlash/Fast Simulation interface: Method for generating hit(s) using the information from fast simulation
    template <typename T> bool Geant4SensitiveAction<T>::processFastSim(const Geant4FastSimSpot* spot, G4TouchableHistory* history)  {
      return Geant4Sensitive::processFastSim(spot, history);
    }

    // Forward declarations
    typedef Geant4HitData::Contribution HitContribution;


  }    // End namespace sim
}      // End namespace dd4hep


#include <DD4hep/Printout.h>
#include <DDG4/Geant4TouchableHandler.h>
#include <DDG4/Geant4StepHandler.h>
#include <DDG4/Geant4VolumeManager.h>
