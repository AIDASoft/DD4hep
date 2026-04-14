"""DDG4 steering file example for AdePT GPU transport integration.

This example demonstrates how to use the Geant4AdePTPhysics and
Geant4AdePTRunAction plugins to offload electromagnetic particle
transport (e-/e+/gamma) to the GPU via AdePT.

Usage:
  ddsim --steeringFile AdePTSteeringFile.py --compactFile <detector.xml>
"""

from DDSim.DD4hepSimulation import DD4hepSimulation

runner = DD4hepSimulation()

# --- Action configuration ---
runner.action.tracker = "Geant4TrackerAction"
runner.action.trackerSDTypes = ["tracker"]
runner.action.calo = "Geant4CalorimeterAction"
runner.action.calorimeterSDTypes = ["calorimeter"]

# --- Output configuration ---
runner.outputConfig.forceDD4HEP = True

# --- Number of events ---
runner.numberOfEvents = 10

# --- Particle gun ---
runner.enableGun = True
runner.gun.particle = "e-"
runner.gun.energy = "10*GeV"
runner.gun.distribution = "uniform"

# --- Field tracking configuration ---
runner.field.delta_chord = 0.025        # mm
runner.field.delta_intersection = 1e-2  # mm
runner.field.delta_one_step = 0.001     # mm
runner.field.eps_min = 5e-5             # mm
runner.field.eps_max = 0.001            # mm
runner.field.min_chord_step = 1e-6      # mm


def setup_physics(kernel):
    """Configure physics list with AdePT GPU transport."""
    from DDG4 import Geant4, PhysicsList

    phys = Geant4(kernel).setupPhysics("FTFP_BERT")

    # Add AdePT physics plugin
    adept_phys = PhysicsList(kernel, str("Geant4AdePTPhysics"))

    # GPU track buffer capacity in millions of slots.
    adept_phys.MillionsOfTrackSlots = 0.1

    # GPU buffer capacity for tracks that leak back to the CPU.
    adept_phys.MillionsOfLeakSlots = 0.1

    # GPU hit buffer capacity in millions of slots.
    adept_phys.MillionsOfHitSlots = 0.1

    # Fraction of the hit buffer occupancy that triggers a flush back to the CPU.
    adept_phys.HitBufferFlushThreshold = 0.1

    # Extra CPU-side capacity factor for returned-track staging and bookkeeping.
    adept_phys.CPUCapacityFactor = 2.5

    # Extra safety margin used when sizing the hit buffer.
    adept_phys.HitBufferSafetyFactor = 1.5

    # Track in all regions on the GPU instead of selecting explicit regions.
    adept_phys.TrackInAllRegions = False

    # Explicit list of Geant4 region names to run on the GPU when TrackInAllRegions is False.
    adept_phys.GPURegionNames = ["EcalRegion"]

    # Region names that should be forced back onto the CPU even if GPU regions are enabled.
    # adept_phys.CPURegionNames = ["SiTrackerBarrelRegion"]

    # Region names that should use Woodcock tracking on the GPU.
    # adept_phys.WDTRegionNames = ["EcalRegion"]

    # Call the user Geant4 stepping action for tracks that return from the GPU.
    adept_phys.CallUserSteppingAction = False

    # Call the user Geant4 tracking action for tracks handled by AdePT.
    adept_phys.CallUserTrackingAction = False

    # When the number of in-flight GPU tracks drops below this value, the remaining
    # tracks are leaked back to Geant4/HepEm on the CPU, terminating the GPU transport
    # loop early.  This avoids many short-lived near-empty kernel launches during the
    # shower tail.  Set to 0 (default) to always finish on the GPU.
    adept_phys.LastNParticlesOnCPU = 0

    # Random seed for AdePT transport.
    adept_phys.AdePTSeed = 42

    # AdePT/plugin verbosity level.
    adept_phys.Verbosity = 1

    # CUDA device stack limit in bytes; larger geometries need more than the default.
    adept_phys.CUDAStackLimit = 8192

    # CUDA device heap limit in bytes; leave unset unless device-side heap allocations are needed.
    # adept_phys.CUDAHeapLimit = 0

    # Maximum number of Woodcock-tracking iterations before falling back.
    adept_phys.MaxWDTIter = 5

    # Kinetic-energy limit for Woodcock tracking in Geant4 energy units.
    adept_phys.WDTKineticEnergyLimit = 0.2

    # Enable multi-step MSC-with-transportation in G4HepEm.
    adept_phys.MultipleStepsInMSC = True

    # Enable energy-loss fluctuations in G4HepEm.
    adept_phys.EnergyLossFluctuation = False

    phys.adopt(adept_phys)
    phys.dump()

    return None


runner.physics.setupUserPhysics(setup_physics)

# Register Geant4AdePTUserParticleHandler as the user particle handler.
# It is adopted at the right time by DDSim's setupUserParticleHandler mechanism,
# which runs inside __setupGeneratorActions during G4RunManager::Initialize().
runner.part.userParticleHandler = "Geant4AdePTUserParticleHandler"
