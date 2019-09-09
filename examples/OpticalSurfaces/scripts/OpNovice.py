#
#
from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/OpticalSurfaces/compact/OpNovice.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  if len(sys.argv) > 1:
    geant4.setupCshUI(macro=sys.argv[1])
  else:
    geant4.setupCshUI()

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.INFO

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugMaterials = True
  act.DebugElements = False
  act.DebugVolumes = True
  act.DebugShapes = True
  act.DebugSurfaces = True

  # Configure I/O
  ###evt_root = geant4.setupROOTOutput('RootOutput','OpNovice_'+time.strftime('%Y-%m-%d_%H-%M'))

  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='gamma', energy=5 * keV, multiplicity=1)
  gun.OutputLevel = generator_output_level

  # And handle the simulation particles.
  """
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100*MeV
  part.OutputLevel = Output.INFO #generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 3.0*m
  user.TrackingVolume_Rmax = 3.0*m
  user.enableUI()
  part.adopt(user)
  """
  geant4.setupTracker('BubbleDevice')

  # Now build the physics list:
  phys = geant4.setupPhysics('')
  ph = DDG4.PhysicsList(kernel, 'Geant4OpticalPhotonPhysics/OpticalGammaPhys')
  ph.VerboseLevel = 2
  ph.addParticleGroup('G4BosonConstructor')
  ph.addParticleGroup('G4LeptonConstructor')
  ph.addParticleGroup('G4MesonConstructor')
  ph.addParticleGroup('G4BaryonConstructor')
  ph.addParticleGroup('G4IonConstructor')
  ph.addParticleConstructor('G4OpticalPhoton')

  ph.addDiscreteParticleProcess('gamma', 'G4GammaConversion')
  ph.addDiscreteParticleProcess('gamma', 'G4ComptonScattering')
  ph.addDiscreteParticleProcess('gamma', 'G4PhotoElectricEffect')
  ph.addParticleProcess(str('e[+-]'), str('G4eMultipleScattering'), -1, 1, 1)
  ph.addParticleProcess(str('e[+-]'), str('G4eIonisation'), -1, 2, 2)
  ph.addParticleProcess(str('e[+-]'), str('G4eBremsstrahlung'), -1, 3, 3)
  ph.addParticleProcess(str('e+'), str('G4eplusAnnihilation'), 0, -1, 4)
  ph.addParticleProcess(str('mu[+-]'), str('G4MuMultipleScattering'), -1, 1, 1)
  ph.addParticleProcess(str('mu[+-]'), str('G4MuIonisation'), -1, 2, 2)
  ph.addParticleProcess(str('mu[+-]'), str('G4MuBremsstrahlung'), -1, 3, 3)
  ph.addParticleProcess(str('mu[+-]'), str('G4MuPairProduction'), -1, 4, 4)
  ph.enableUI()
  phys.adopt(ph)

  ph = DDG4.PhysicsList(kernel, 'Geant4ScintillationPhysics/ScintillatorPhys')
  ph.ScintillationYieldFactor = 1.0
  ph.ScintillationExcitationRatio = 1.0
  ph.TrackSecondariesFirst = False
  ph.VerboseLevel = 2
  ph.enableUI()
  phys.adopt(ph)

  ph = DDG4.PhysicsList(kernel, 'Geant4CerenkovPhysics/CerenkovPhys')
  ph.MaxNumPhotonsPerStep = 10
  ph.MaxBetaChangePerStep = 10.0
  ph.TrackSecondariesFirst = True
  ph.VerboseLevel = 2
  ph.enableUI()
  phys.adopt(ph)

  phys.dump()

  geant4.execute()


if __name__ == "__main__":
  run()
