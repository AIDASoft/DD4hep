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
#include "DDG4/Geant4GeneratorWrapper.h"

#include "DD4hep/InstanceCount.h"
#include "DD4hep/Plugins.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4InputHandling.h"

// Geant4 include files
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4VPrimaryGenerator.hh"

// C/C++ include files
#include <stdexcept>
#include <set>

using namespace dd4hep::sim;
using namespace std;

/// Standard constructor
Geant4GeneratorWrapper::Geant4GeneratorWrapper(Geant4Context* ctxt, const string& nam)
  : Geant4GeneratorAction(ctxt,nam), m_generator(0)
{
  declareProperty("Uses", m_generatorType);
  declareProperty("Mask", m_mask = 1);
  InstanceCount::increment(this);
}
  
/// Default destructor
Geant4GeneratorWrapper::~Geant4GeneratorWrapper()  {
  detail::deletePtr(m_generator);
  InstanceCount::decrement(this);
}

G4VPrimaryGenerator* Geant4GeneratorWrapper::generator()   {
  if ( 0 == m_generator )  {
    m_generator = PluginService::Create<G4VPrimaryGenerator*>(m_generatorType);
    if ( 0 == m_generator )  {
      PluginDebug dbg;
      m_generator = PluginService::Create<G4VPrimaryGenerator*>(m_generatorType);
      if ( !m_generator )  {
        throw runtime_error("Geant4GeneratorWrapper: FATAL Failed to "
                            "create G4VPrimaryGenerator of type " + m_generatorType + ".");
      }
    }
  }
  return m_generator;
}

/// Event generation action callback
void Geant4GeneratorWrapper::operator()(G4Event* event)  {
  Geant4PrimaryEvent* prim = context()->event().extension<Geant4PrimaryEvent>();
  Geant4PrimaryMap*   primaryMap = context()->event().extension<Geant4PrimaryMap>();
  set<G4PrimaryVertex*> primaries;
  
  /// Collect all existing interactions (primary vertices)
  for(G4PrimaryVertex* v=event->GetPrimaryVertex(); v; v=v->GetNext())
    primaries.insert(v);

  // Now generate the new interaction
  generator()->GeneratePrimaryVertex(event);

  /// Add all the missing interactions (primary vertices) to the primary event record.
  for(G4PrimaryVertex* gv=event->GetPrimaryVertex(); gv; gv=gv->GetNext())  {
    if ( primaries.find(gv) == primaries.end() )   {
      Geant4PrimaryInteraction* inter = createPrimary(m_mask, primaryMap, gv);
      prim->add(m_mask, inter);
    }
  }
}
