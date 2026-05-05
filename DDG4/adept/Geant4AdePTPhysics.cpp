//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//==========================================================================

/// @file Geant4AdePTPhysics.cpp
/// @brief DDG4 physics list plugin for AdePT GPU transport integration.
///
/// This plugin bridges DD4hep's DDG4 action system with AdePT's GPU-accelerated
/// electromagnetic particle transport. It creates an AdePTConfiguration and
/// AdePTTrackingManager, registers them for e-/e+/gamma, and exposes AdePT
/// configuration parameters as DD4hep properties.

#include <string>
#include <vector>

#include <DD4hep/Detector.h>
#include <DD4hep/FieldTypes.h>
#include <DDG4/Factories.h>
#include <DDG4/Geant4Action.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4PhysicsList.h>

#include <G4Electron.hh>
#include <G4Gamma.hh>
#include <G4MTRunManager.hh>
#include <G4Positron.hh>
#include <G4RunManager.hh>
#include <G4Track.hh>
#include <G4VModularPhysicsList.hh>
#include <G4VUserPhysicsList.hh>

#include <AdePT/g4integration/AdePTConfiguration.hh>
#include <AdePT/g4integration/AdePTTrackingManager.hh>

#include <G4HepEmConfig.hh>

namespace dd4hep { namespace sim {

  /// DDG4 action plugin for AdePT tracking manager integration.
  class Geant4AdePTPhysics : public Geant4PhysicsList {
  public:
    Geant4AdePTPhysics(Geant4Context* ctxt, std::string const& name);

    DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4AdePTPhysics);

    /// constructPhysics is called during configure(), before G4RunManager::Initialize().
    /// We override it only to satisfy the interface; the actual setup is deferred.
    void constructPhysics(G4VModularPhysicsList* /*physics*/) override {}

    /// constructProcesses is called from ConstructProcess() during G4RunManager::Initialize(),
    /// after the geometry and world volume are set up.  AdePTTrackingManager's base class
    /// (G4HepEmTrackingManager) calls G4SafetyHelper::InitialiseHelper() in its constructor,
    /// which requires a live navigator world, so instantiation must happen here.
    void constructProcesses(G4VUserPhysicsList* physics) override;

  private:
    /// GPU buffer sizing
    double m_millionsOfTrackSlots{1.0};
    double m_millionsOfLeakSlots{1.0};
    double m_millionsOfHitSlots{1.0};
    float  m_hitBufferFlushThreshold{0.8f};
    float  m_cpuCapacityFactor{2.5f};
    double m_hitBufferSafetyFactor{1.5};

    /// GPU region configuration
    bool m_trackInAllRegions{false};
    std::vector<std::string> m_gpuRegionNames{};
    std::vector<std::string> m_cpuRegionNames{};
    std::vector<std::string> m_wdtRegionNames{};

    /// User action callbacks
    bool m_callUserSteppingAction{false};

    /// Finish last N particles on CPU (0 = always finish on GPU)
    int m_lastNParticlesOnCPU{0};

    /// Speed-of-light mode: kill all e-/e+/gamma immediately (benchmark/debug only)
    bool m_speedOfLight{false};

    /// Random seed and verbosity
    int m_adeptSeed{1234567};
    int m_verbosity{0};

    /// CUDA device limits
    int m_cudaStackLimit{0};
    int m_cudaHeapLimit{0};

    /// Woodcock tracking
    int    m_maxWDTIter{5};
    double m_wdtKineticEnergyLimit{0.2};

    /// G4HepEm options
    bool m_multipleStepsInMSC{false};
    bool m_energyLossFluctuation{false};

    /// Apply DD4hep properties to AdePTConfiguration
    void configureAdePT(AdePTConfiguration& config);
  };

  //---------------------------------------------------------------------------

  Geant4AdePTPhysics::Geant4AdePTPhysics(Geant4Context* ctxt, std::string const& name)
    : Geant4PhysicsList(ctxt, name)
  {
    declareProperty("MillionsOfTrackSlots",    m_millionsOfTrackSlots);
    declareProperty("MillionsOfLeakSlots",     m_millionsOfLeakSlots);
    declareProperty("MillionsOfHitSlots",      m_millionsOfHitSlots);
    declareProperty("HitBufferFlushThreshold", m_hitBufferFlushThreshold);
    declareProperty("CPUCapacityFactor",       m_cpuCapacityFactor);
    declareProperty("HitBufferSafetyFactor",   m_hitBufferSafetyFactor);
    declareProperty("TrackInAllRegions",       m_trackInAllRegions);
    declareProperty("GPURegionNames",          m_gpuRegionNames);
    declareProperty("CPURegionNames",          m_cpuRegionNames);
    declareProperty("WDTRegionNames",          m_wdtRegionNames);
    declareProperty("CallUserSteppingAction",  m_callUserSteppingAction);
    declareProperty("LastNParticlesOnCPU",     m_lastNParticlesOnCPU);
    declareProperty("SpeedOfLight",            m_speedOfLight);
    declareProperty("AdePTSeed",               m_adeptSeed);
    declareProperty("Verbosity",               m_verbosity);
    declareProperty("CUDAStackLimit",          m_cudaStackLimit);
    declareProperty("CUDAHeapLimit",           m_cudaHeapLimit);
    declareProperty("MaxWDTIter",              m_maxWDTIter);
    declareProperty("WDTKineticEnergyLimit",   m_wdtKineticEnergyLimit);
    declareProperty("MultipleStepsInMSC",      m_multipleStepsInMSC);
    declareProperty("EnergyLossFluctuation",   m_energyLossFluctuation);
  }

  //---------------------------------------------------------------------------

  void Geant4AdePTPhysics::configureAdePT(AdePTConfiguration& config)
  {
    config.SetMillionsOfTrackSlots(m_millionsOfTrackSlots);
    config.SetMillionsOfLeakSlots(m_millionsOfLeakSlots);
    config.SetMillionsOfHitSlots(m_millionsOfHitSlots);
    config.SetHitBufferFlushThreshold(m_hitBufferFlushThreshold);
    config.SetCPUCapacityFactor(m_cpuCapacityFactor);
    config.SetHitBufferSafetyFactor(m_hitBufferSafetyFactor);
    config.SetTrackInAllRegions(m_trackInAllRegions);
    config.SetCallUserSteppingAction(m_callUserSteppingAction);
    config.SetCallUserTrackingAction(true);
    config.SetAdePTSeed(m_adeptSeed);
    config.SetVerbosity(m_verbosity);
    config.SetLastNParticlesOnCPU(m_lastNParticlesOnCPU);
    config.SetSpeedOfLight(m_speedOfLight);

    if (m_cudaStackLimit > 0)
      config.SetCUDAStackLimit(m_cudaStackLimit);
    if (m_cudaHeapLimit > 0)
      config.SetCUDAHeapLimit(m_cudaHeapLimit);

    config.SetMaxWDTIter(m_maxWDTIter);
    config.SetWDTKineticEnergyLimit(m_wdtKineticEnergyLimit);
    config.SetMultipleStepsInMSCWithTransportation(m_multipleStepsInMSC);
    config.SetEnergyLossFluctuation(m_energyLossFluctuation);

    for (auto const& region : m_gpuRegionNames)
      config.AddGPURegionName(region);
    for (auto const& region : m_cpuRegionNames)
      config.RemoveGPURegionName(region);
    for (auto const& region : m_wdtRegionNames)
      config.AddWDTRegionName(region);

    // AdePTTrackingManager::InitializeAdePT() needs the number of worker threads
    // to be pre-set in AdePTConfiguration when G4MTRunManager is not available
    // (sequential mode).  Without this, it throws a std::runtime_error, leaving
    // fAdeptTransport null and causing a segfault in HandOverOneTrack.
    if (config.GetNumThreads() <= 0) {
      int nThreads = 1; // default: sequential mode
      if (auto* mtRM = G4MTRunManager::GetMasterRunManager())
        nThreads = mtRM->GetNumberOfThreads();
      config.SetNumThreads(nThreads);
      info("AdePT: configured for %d thread(s)", nThreads);
    }
  }

  //---------------------------------------------------------------------------

  void Geant4AdePTPhysics::constructProcesses(G4VUserPhysicsList* /*physics_list*/)
  {
    // Create AdePT configuration and apply DD4hep properties
    auto* config = new AdePTConfiguration();
    this->configureAdePT(*config);

    // Create tracking manager and register for EM particles
    auto* trackingManager = new AdePTTrackingManager(config, m_verbosity);

    // Configure G4HepEm options
    auto* g4hepemconfig = trackingManager->GetG4HepEmConfig();
    g4hepemconfig->SetMultipleStepsInMSCWithTransportation(m_multipleStepsInMSC);
    g4hepemconfig->SetEnergyLossFluctuation(m_energyLossFluctuation);

    for (auto const& regionName : m_wdtRegionNames) {
      g4hepemconfig->SetWoodcockTrackingRegion(regionName);
    }
    g4hepemconfig->SetWDTEnergyLimit(m_wdtKineticEnergyLimit);

    // Register tracking manager for e-, e+, and gamma
    G4Electron::Definition()->SetTrackingManager(trackingManager);
    G4Positron::Definition()->SetTrackingManager(trackingManager);
    G4Gamma::Definition()->SetTrackingManager(trackingManager);

    info("Registered AdePT tracking manager for e-/e+/gamma");
    info("  MillionsOfTrackSlots: %.1f, MillionsOfHitSlots: %.1f",
         m_millionsOfTrackSlots, m_millionsOfHitSlots);
    if (!m_gpuRegionNames.empty()) {
      for (auto const& r : m_gpuRegionNames)
        info("  GPU region: %s", r.c_str());
    }
    if (m_trackInAllRegions)
      info("  Tracking in all regions");
  }

}} // namespace dd4hep::sim

DECLARE_GEANT4ACTION(Geant4AdePTPhysics)
