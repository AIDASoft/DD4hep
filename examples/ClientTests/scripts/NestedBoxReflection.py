#
#
from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import logging
import DDG4
from DDG4 import OutputLevel as Output
from g4units import MeV, GeV, m, mm
#
#
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def help():
  logging.info("Check_shape.py -option [-option]                           ")
  logging.info("       -vis                          Enable visualization  ")
  logging.info("       -batch                        Batch execution       ")


def run():
  hlp = False
  vis = False
  dump = False
  batch = False
  install_dir = os.environ['DD4hepINSTALL']
  #
  for i in list(range(len(sys.argv))):
    c = sys.argv[i].upper()
    if c.find('BATCH') < 2 and c.find('BATCH') >= 0:
      batch = True
    elif c[:4] == '-VIS':
      vis = True
    elif c[:4] == '-DUM':
      dump = True
    elif c[:2] == '-H':
      hlp = True

  if hlp:
    help()
    sys.exit(1)

  kernel = DDG4.Kernel()
  description = kernel.detectorDescription()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  #
  logger.info("#  Configure UI")
  ui = None
  if batch:
    geant4.setupCshUI(ui=None, vis=None)
    kernel.UI = 'UI'
  else:
    ui = geant4.setupCshUI(vis=vis)

  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/NestedBoxReflection.xml"))
  DDG4.importConstants(description)

  geant4.printDetectors()
  if dump:
    seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
    act.DebugReflections = True
    act.DebugMaterials = False
    act.DebugElements = False
    act.DebugVolumes = False
    act.DebugShapes = False
    act.DumpHierarchy = ~0x0

  logger.info("#  Configure G4 magnetic field tracking")
  geant4.setupTrackingField()

  logger.info("#  Setup random generator")
  rndm = DDG4.Action(kernel, 'Geant4Random/Random')
  rndm.Seed = 987654321
  rndm.initialize()
  #
  logger.info("#  Configure Event actions")
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputType = 3  # Print both: table and tree
  prt.OutputLevel = Output.INFO
  kernel.eventAction().adopt(prt)
  #
  logger.info("#  Configure I/O")
  geant4.setupROOTOutput('RootOutput', 'BoxReflect_' + time.strftime('%Y-%m-%d_%H-%M'))
  #
  gen = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  #
  logger.info("#  Generation of isotrope tracks of a given multiplicity with overlay:")
  gen = DDG4.GeneratorAction(kernel, "Geant4ParticleGun/IsotropE+")
  gen.mask = 4
  gen.isotrop = True
  gen.particle = 'e+'
  gen.energy = 100 * GeV
  gen.multiplicity = 200
  gen.position = (0 * m, 0 * m, 0 * m)
  gen.direction = (0, 0, 1.)
  gen.distribution = 'uniform'
  gen.standalone = False
  # gen.PhiMin = 0.0*rad
  # gen.PhiMax = 2.0*math.pi*rad
  # gen.ThetaMin = 0.0*math.pi*rad
  # gen.ThetaMax = 1.0*math.pi*rad
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  #
  logger.info("#  Merge all existing interaction records")
  gen = DDG4.GeneratorAction(kernel, "Geant4InteractionMerger/InteractionMerger")
  gen.OutputLevel = 4  # generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  #
  logger.info("#  Finally generate Geant4 primaries")
  gen = DDG4.GeneratorAction(kernel, "Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = 4  # generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  #
  logger.info("#  ....and handle the simulation particles.")
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  # part.SaveProcesses = ['conv','Decay']
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100 * MeV
  part.OutputLevel = 5  # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 3.0 * m
  user.TrackingVolume_Rmax = 3.0 * m
  user.enableUI()
  part.adopt(user)
  #
  logger.info("#  Now setup the calorimeters")
  seq, actions = geant4.setupDetectors()
  #
  logger.info("#  Now build the physics list:")
  geant4.setupPhysics('QGSP_BERT')
  ph = geant4.addPhysics(str('Geant4PhysicsList/Myphysics'))
  ph.addPhysicsConstructor(str('G4StepLimiterPhysics'))
  #
  # Add special particle types from specialized physics constructor
  part = geant4.addPhysics('Geant4ExtraParticles/ExtraParticles')
  part.pdgfile = os.path.join(install_dir, 'examples/DDG4/examples/particle.tbl')
  #
  # Add global range cut
  rg = geant4.addPhysics('Geant4DefaultRangeCut/GlobalRangeCut')
  rg.RangeCut = 0.7 * mm
  #
  #
  if ui and vis:
    cmds = []
    cmds.append('/control/verbose 2')
    cmds.append('/run/initialize')
    cmds.append('/vis/open OGL')
    cmds.append('/vis/verbose errors')
    cmds.append('/vis/drawVolume')
    cmds.append('/vis/viewer/set/viewpointThetaPhi 55. 45.')
    cmds.append('/vis/scene/add/axes 0 0 0 3 m')
    ui.Commands = cmds

  kernel.configure()
  kernel.initialize()

  # DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
