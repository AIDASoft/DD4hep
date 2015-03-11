// $Id: $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#ifndef DD4HEP_DDG4_GEANT4SENSDETACTION_H
#include "DDG4/Geant4SensDetAction.h"
#endif
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Data.h"

namespace DD4hep {
  namespace Simulation   {

    /// Standard , initializing constructor
    template <typename T>
    Geant4SensitiveAction<T>::Geant4SensitiveAction(Geant4Context* ctxt,
						    const std::string& nam,
						    Geometry::DetElement det,
						    Geometry::LCDD& lcdd_ref)
      : Geant4Sensitive(ctxt,nam,det,lcdd_ref), m_collectionID(0)
    {
      initialize();
      defineCollections();
      InstanceCount::increment(this);
    }

    /// Default destructor
    template <typename T> Geant4SensitiveAction<T>::~Geant4SensitiveAction()  {
      finalize();
      InstanceCount::decrement(this);
    }

    /// Initialization overload for specialization
    template <typename T> void Geant4SensitiveAction<T>::initialize()    {
    }

    /// Finalization overload for specialization
    template <typename T> void Geant4SensitiveAction<T>::finalize()    {
    }

    /// G4VSensitiveDetector interface: Method invoked at the begining of each event.
    template <typename T> void Geant4SensitiveAction<T>::begin(G4HCofThisEvent* hce)  {
      Geant4Sensitive::begin(hce);
    }

    /// G4VSensitiveDetector interface: Method invoked at the end of each event.
    template <typename T> void Geant4SensitiveAction<T>::end(G4HCofThisEvent* hce)  {
      Geant4Sensitive::end(hce);
    }

    /// G4VSensitiveDetector interface: Method for generating hit(s) using the G4Step object.
    template <typename T> bool Geant4SensitiveAction<T>::process(G4Step* step,G4TouchableHistory* history)  {
      return Geant4Sensitive::process(step,history);
    }

    /// G4VSensitiveDetector interface: Method invoked if the event was aborted.
    template <typename T> void Geant4SensitiveAction<T>::clear(G4HCofThisEvent* hce)  {
      Geant4Sensitive::clear(hce);
    }

    // Forward declarations
    typedef Geant4HitData::Contribution HitContribution;
  }
}


#include "DD4hep/Printout.h"
#include "DDG4/Geant4TouchableHandler.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4VolumeManager.h"
