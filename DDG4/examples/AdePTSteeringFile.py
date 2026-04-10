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

    # CUDA device stack limit (bytes). VecGeom navigation requires more than the
    # default 1024 bytes per thread; 8192 is a safe value for complex geometries.
    adept_phys.CUDAStackLimit = 8192

    # GPU buffer sizing (in millions of slots).
    # Reduce these for GPUs with limited VRAM (e.g. 2GB).
    adept_phys.MillionsOfTrackSlots = 0.1
    adept_phys.MillionsOfLeakSlots = 0.1
    adept_phys.MillionsOfHitSlots = 0.1
    adept_phys.HitBufferFlushThreshold = 0.8

    # Track in all regions or specify GPU regions
    adept_phys.TrackInAllRegions = True
    # Alternatively, specify specific GPU regions:
    # adept_phys.GPURegionNames = ["EMCalorimeter", "HCalorimeter"]
    # adept_phys.WDTRegionNames = ["DenseDetector"]

    # Random seed
    adept_phys.AdePTSeed = 42

    # Verbosity
    adept_phys.Verbosity = 1

    phys.adopt(adept_phys)
    phys.dump()
    return None


runner.physics.setupUserPhysics(setup_physics)

runner.part.userParticleHandler = ""
