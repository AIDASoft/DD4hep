#
#
from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV, MeV, m
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
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/SiliconBlock.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  if len(sys.argv) > 1:
    geant4.setupCshUI(macro=sys.argv[1])
  else:
    geant4.setupCshUI()

  # Configure field
  geant4.setupTrackingField(prt=True)
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

  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'SiliconBlock_' + time.strftime('%Y-%m-%d_%H-%M'))

  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='mu-', energy=20 * GeV, multiplicity=1)
  gun.OutputLevel = generator_output_level

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100 * MeV
  part.OutputLevel = Output.INFO  # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 3.0 * m
  user.TrackingVolume_Rmax = 3.0 * m
  user.enableUI()
  part.adopt(user)

  geant4.setupTracker('SiliconBlockUpper')
  geant4.setupTracker('SiliconBlockDown')

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, str('Geant4PhysicsList/Myphysics'))
  ph.addParticleConstructor(str('G4Geantino'))
  ph.addParticleConstructor(str('G4BosonConstructor'))
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  geant4.execute()


if __name__ == "__main__":
  run()
