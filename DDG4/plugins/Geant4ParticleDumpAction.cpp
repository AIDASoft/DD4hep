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
#ifndef DD4HEP_DDG4_GEANT4PARTICLEDUMPACTION_H
#define DD4HEP_DDG4_GEANT4PARTICLEDUMPACTION_H

// Framework include files
#include "DDG4/Geant4EventAction.h"

// Forward declarations
class G4VHitsCollection;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    // Forward declarations
    class Geant4ParticleMap;
    
    /// Class to measure the energy of escaping tracks
    /** Class to measure the energy of escaping tracks of a detector using Geant 4
     * Measure escaping energy....
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ParticleDumpAction : public Geant4EventAction {
    public:
      /// Standard constructor
      Geant4ParticleDumpAction(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4ParticleDumpAction();
      /// Geant4EventAction interface: Begin-of-event callback
      virtual void begin(const G4Event* event)  override;
      /// Geant4EventAction interface: End-of-event callback
      virtual void end(const G4Event* event)  override;
    };

  }    // End namespace Simulation
}      // End namespace DD4hep

#endif /* DD4HEP_DDG4_GEANT4PARTICLEDUMPACTION_H */

//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4DataDump.h"
#include "DDG4/Geant4HitCollection.h"

// Geant 4 includes
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Standard constructor
Geant4ParticleDumpAction::Geant4ParticleDumpAction(Geant4Context* ctxt, const string& nam)
  : Geant4EventAction(ctxt, nam)
{
  m_needsControl = true;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ParticleDumpAction::~Geant4ParticleDumpAction() {
  InstanceCount::decrement(this);
}

/// Geant4EventAction interface: Begin-of-event callback
void Geant4ParticleDumpAction::begin(const G4Event* /* event */)   {
}

/// Geant4EventAction interface: End-of-event callback
void Geant4ParticleDumpAction::end(const G4Event* event)    {
  Geant4ParticleMap* parts = context()->event().extension<Geant4ParticleMap>();
  if ( parts )   {
    Geant4DataDump dump(name());
    dump.print(ALWAYS,parts);
    return;
  }
  warning("+++ [Event:%d] No particle map available!",event->GetEventID());
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4ParticleDumpAction)
