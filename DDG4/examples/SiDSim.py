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
from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import logging
import DDG4
from DDG4 import OutputLevel as Output
from g4units import keV, GeV, mm, ns, MeV
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
  logging.info("SiDSim.py -option [-option]                           ")
  logging.info("       -vis                     Enable visualization  ")
  logging.info("       -batch                   Batch execution       ")


def run():
  hlp = False
  vis = False
  batch = False
  #
  for i in list(range(len(sys.argv))):
    c = sys.argv[i].upper()
    if c.find('BATCH') < 2 and c.find('BATCH') >= 0:
      batch = True
    elif c[:4] == '-VIS':
      vis = True
    elif c[:2] == '-H':
      hlp = True

  if hlp:
    help()
    sys.exit(1)

  kernel = DDG4.Kernel()
  description = kernel.detectorDescription()
  install_dir = os.environ['DD4hepINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/DDDetectors/compact/SiD.xml"))
  DDG4.importConstants(description)

  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  logger.info("#  Configure UI")
  ui = None
  if batch:
    geant4.setupCshUI(ui=None, vis=None)
    kernel.UI = 'UI'
  else:
    ui = geant4.setupCshUI(vis=vis)

  logger.info("#  Configure G4 magnetic field tracking")
  geant4.setupTrackingField()

  logger.info("#  Setup random generator")
  rndm = DDG4.Action(kernel, 'Geant4Random/Random')
  rndm.Seed = 987654321
  rndm.initialize()
  # rndm.showStatus()

  logger.info("#  Configure Run actions")
  run1 = DDG4.RunAction(kernel, 'Geant4TestRunAction/RunInit')
  run1.Property_int = 12345
  run1.Property_double = -5e15 * keV
  run1.Property_string = 'Startrun: Hello_2'
  logger.info("%s %s %s", run1.Property_string, str(run1.Property_double), str(run1.Property_int))
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().adopt(run1)

  logger.info("#  Configure Event actions")
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  logger.info("""
  Configure I/O
  """)
  # evt_lcio = geant4.setupLCIOOutput('LcioOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  # evt_lcio.OutputLevel = Output.ERROR

  geant4.setupROOTOutput('RootOutput', 'CLICSiD_' + time.strftime('%Y-%m-%d_%H-%M'))

  gen = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)

  # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
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
  gen = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexSmear/SmearPi+")
  gen.Mask = 1
  gen.Offset = (20 * mm, 10 * mm, 10 * mm, 0 * ns)
  gen.Sigma = (4 * mm, 1 * mm, 1 * mm, 0 * ns)
  kernel.generatorAction().adopt(gen)

  logger.info("#  Second particle generator: e-")
  gen = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropE-")
  gen.Mask = 2
  gen.Particle = 'e-'
  gen.Energy = 25 * GeV
  gen.Multiplicity = 2
  gen.Distribution = 'uniform'
  kernel.generatorAction().adopt(gen)
  logger.info("  Install vertex smearing for this interaction")
  gen = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexSmear/SmearE-")
  gen.Mask = 2
  gen.Offset = (-20 * mm, -10 * mm, -10 * mm, 0 * ns)
  gen.Sigma = (12 * mm, 8 * mm, 8 * mm, 0 * ns)
  kernel.generatorAction().adopt(gen)

  logger.info("#  Second particle generator: mu+")
  gen = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropMu+")
  gen.Mask = 3
  gen.Particle = 'mu+'
  gen.Energy = 100 * GeV
  gen.Multiplicity = 3
  gen.Distribution = 'uniform'
  kernel.generatorAction().adopt(gen)
  # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
  user.TrackingVolume_Zmax = DDG4.EcalEndcap_zmin
  user.TrackingVolume_Rmax = DDG4.EcalBarrel_rmin
  user.enableUI()
  part.adopt(user)
  #
  logger.info("#  Setup global filters fur use in sensitive detectors")
  f1 = DDG4.Filter(kernel, 'GeantinoRejectFilter/GeantinoRejector')
  f2 = DDG4.Filter(kernel, 'ParticleRejectFilter/OpticalPhotonRejector')
  f2.particle = 'opticalphoton'
  f3 = DDG4.Filter(kernel, 'ParticleSelectFilter/OpticalPhotonSelector')
  f3.particle = 'opticalphoton'
  f4 = DDG4.Filter(kernel, 'EnergyDepositMinimumCut')
  f4.Cut = 10 * MeV
  f4.enableUI()
  kernel.registerGlobalFilter(f1)
  kernel.registerGlobalFilter(f2)
  kernel.registerGlobalFilter(f3)
  kernel.registerGlobalFilter(f4)
  #
  logger.info("#  First the tracking detectors")
  seq, act = geant4.setupTracker('SiVertexBarrel')
  seq.adopt(f1)
  act.adopt(f1)
  #
  seq, act = geant4.setupTracker('SiVertexEndcap')
  seq.adopt(f1)
  #
  seq, act = geant4.setupTracker('SiTrackerBarrel')
  seq, act = geant4.setupTracker('SiTrackerEndcap')
  seq, act = geant4.setupTracker('SiTrackerForward')
  logger.info("#  Now setup the calorimeters")
  seq, act = geant4.setupCalorimeter('EcalBarrel')
  seq, act = geant4.setupCalorimeter('EcalEndcap')
  seq, act = geant4.setupCalorimeter('HcalBarrel')
  seq, act = geant4.setupCalorimeter('HcalEndcap')
  seq, act = geant4.setupCalorimeter('HcalPlug')
  seq, act = geant4.setupCalorimeter('MuonBarrel')
  seq, act = geant4.setupCalorimeter('MuonEndcap')
  seq, act = geant4.setupCalorimeter('LumiCal')
  seq, act = geant4.setupCalorimeter('BeamCal')
  #
  logger.info("#  Now build the physics list:")
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = geant4.addPhysics(str('Geant4PhysicsList/Myphysics'))
  ph.addPhysicsConstructor(str('G4StepLimiterPhysics'))
  #
  # Add special particle types from specialized physics constructor
  part = geant4.addPhysics(str('Geant4ExtraParticles/ExtraParticles'))
  part.pdgfile = os.path.join(install_dir, 'examples/DDG4/examples/particle.tbl')
  #
  # Add global range cut
  rg = geant4.addPhysics(str('Geant4DefaultRangeCut/GlobalRangeCut'))
  rg.RangeCut = 0.7 * mm
  #
  phys.dump()
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
    cmds.append('/vis/scene/add/axes 0 0 0 10 m')
    ui.Commands = cmds

  kernel.configure()
  kernel.initialize()

  # DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
