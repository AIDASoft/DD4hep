# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
#
#
import os
import time
import DDG4
import logging
from g4units import GeV, MeV, m
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  args = DDG4.CommandLine()
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/DriftChamber.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.registerInterruptHandler()
  geant4.printDetectors()
  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro)
  else:
    ui = geant4.setupCshUI()

  cmds = []
  if args.verbose:
    cmds.append('/run/verbose ' + str(args.verbose))

  if args.events:
    cmds.append('/run/beamOn ' + str(args.events))
    cmds.append('/ddg4/UI/terminate')

  if len(cmds) > 0:
    ui.Commands = cmds

  logger.info("#  Configure G4 magnetic field tracking")
  geant4.setupTrackingField()

  logger.info("#  Setup random generator")
  rndm = DDG4.Action(kernel, 'Geant4Random/Random')
  rndm.Seed = 987654321
  if args.seed_time:
    rndm.Seed = int(time.time())
  rndm.initialize()

  gen = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)

  logger.info("""
  Generation of isotrope tracks of a given multiplicity with overlay:
  """)
  logger.info("#  First particle generator: pi+")
  gen = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropPi+")
  gen.Mask = 1
  gen.Particle = 'pi+'
  gen.Energy = 100 * GeV
  gen.Multiplicity = 2
  gen.Distribution = 'cos(theta)'
  kernel.generatorAction().adopt(gen)
  logger.info("#  Install vertex smearing for this interaction")

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
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100 * MeV
  part.OutputLevel = 5  # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 1.5 * m
  user.TrackingVolume_Rmax = 1.5 * m
  user.enableUI()
  part.adopt(user)
  #
  seq, act = geant4.setupTracker('DriftChamber')
  #
  logger.info("#  Now build the physics list:")
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = geant4.addPhysics(str('Geant4PhysicsList/Myphysics'))
  ph.addPhysicsConstructor(str('G4StepLimiterPhysics'))
  #
  phys.dump()
  #
  kernel.configure()
  kernel.initialize()

  # DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
