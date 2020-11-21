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
"""
   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
from __future__ import absolute_import, unicode_literals
import logging
import math
import time
import sys
import os
from g4units import rad, GeV, MeV, mm, m
from ddsix.moves import range

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def help():
  logging.info("Check_shape.py -option [-option]                           ")
  logging.info("       -geometry   <geometry file>   Geometry file         ")
  logging.info("       -vis                          Enable visualization  ")
  logging.info("       -batch                        Batch execution       ")


def run():
  geo = None
  vis = False
  batch = False
  install_dir = os.environ['DD4hepINSTALL']
  for i in list(range(len(sys.argv))):
    c = sys.argv[i].upper()
    if c.find('BATCH') < 2 and c.find('BATCH') >= 0:
      batch = True
    elif c[:4] == '-GEO':
      geo = sys.argv[i + 1]
    elif c[:4] == '-VIS':
      vis = True

  if not geo:
    help()
    sys.exit(1)

  import DDG4
  Output = DDG4.OutputLevel
  kernel = DDG4.Kernel()
  # Configure UI
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  if batch:
    ui = geant4.setupCshUI(ui=None, vis=None)
    kernel.UI = 'UI'
  else:
    ui = geant4.setupCshUI(vis=vis)
  kernel.loadGeometry(geo)
  # Configure field
  geant4.setupTrackingField(prt=True)

  logger.info("#  Setup random generator")
  rndm = DDG4.Action(kernel, 'Geant4Random/Random')
  rndm.Seed = 987654321
  rndm.initialize()
  #
  # Setup detector
  seq, act = geant4.setupDetectors()
  #
  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'CheckShape_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=True)
  #
  # Setup particle gun
  geant4.setupGun(name="Gun",
                  particle='e-',
                  energy=100 * GeV,
                  isotrop=True,
                  multiplicity=1,
                  position=(0 * m, 0 * m, 0 * m),
                  PhiMin=0.0 * rad,
                  PhiMax=math.pi * 2.0 * rad,
                  ThetaMin=0.0 * rad,
                  ThetaMax=math.pi * rad)
  #
  prt = DDG4.EventAction(kernel, str('Geant4ParticlePrint/ParticlePrint'))
  prt.OutputLevel = Output.INFO
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)
  part = DDG4.GeneratorAction(kernel, str('Geant4ParticleHandler/ParticleHandler'))
  kernel.generatorAction().adopt(part)
  part.MinimalKineticEnergy = 100 * MeV
  part.SaveProcesses = ['Decay']
  part.OutputLevel = 5  # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel, str('Geant4TCUserParticleHandler/UserParticleHandler'))
  user.TrackingVolume_Rmax = 3.0 * m
  user.TrackingVolume_Zmax = 2.0 * m
  user.enableUI()
  part.adopt(user)
  #
  #
  prt = DDG4.EventAction(kernel, str('Geant4ParticlePrint/ParticlePrint'))
  prt.OutputLevel = Output.INFO
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)
  #
  # Now build the physics list:
  phys = geant4.setupPhysics(str('QGSP_BERT'))
  ph = geant4.addPhysics(str('Geant4PhysicsList/Myphysics'))
  ph.addPhysicsConstructor(str('G4StepLimiterPhysics'))
  ph.addParticleConstructor(str('G4Geantino'))
  ph.addParticleConstructor(str('G4BosonConstructor'))
  #
  # Add special particle types from specialized physics constructor
  part = geant4.addPhysics('Geant4ExtraParticles/ExtraParticles')
  part.pdgfile = os.path.join(install_dir, 'examples/DDG4/examples/particle.tbl')
  #
  # Add global range cut
  rg = geant4.addPhysics('Geant4DefaultRangeCut/GlobalRangeCut')
  rg.RangeCut = 0.7 * mm
  #
  phys.dump()
  #
  cmds = []
  if vis:
    cmds.append('/control/verbose 2')
    cmds.append('/run/initialize')
    cmds.append('/vis/open OGL')
    cmds.append('/vis/verbose errors')
    cmds.append('/vis/drawVolume')
    cmds.append('/vis/viewer/set/viewpointThetaPhi 55. 11.')
    cmds.append('/vis/scene/add/axes 0 0 0 1 m')
  #else:
  #  cmds.append('/ddg4/ConstructGeometry/printVolume /world_volume_1/Shape_Test_0/Shape_Test_vol_0_0')
  #  cmds.append('/ddg4/UI/exit')
  ui.Commands = cmds
  kernel.NumEvents = 0
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
