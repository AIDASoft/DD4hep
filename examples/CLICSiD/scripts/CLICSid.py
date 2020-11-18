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
from __future__ import absolute_import, unicode_literals
import sys
import logging
import DDG4

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


class CLICSid:
  def __init__(self, tracker='Geant4TrackerCombineAction', no_physics=True):
    self.kernel = DDG4.Kernel()
    self.description = self.kernel.detectorDescription()
    self.geant4 = DDG4.Geant4(self.kernel, tracker=tracker)
    self.kernel.UI = ""
    if no_physics:
      self.noPhysics()

  def loadGeometry(self, file=None):
    import os
    if file is None:
      install_dir = os.environ['DD4hepINSTALL']
      level = DDG4.printLevel()
      DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
      self.kernel.loadGeometry(str("file:" + install_dir + "/DDDetectors/compact/SiD.xml"))
      DDG4.setPrintLevel(level)
    else:
      ui = DDG4.DD4hepUI(self.description)
      ui.importROOT(file)
    return self

  # Example to show how to configure G4 magnetic field tracking
  def setupField(self, quiet=True):
    return self.geant4.setupTrackingField(prt=True)

  # Example to show how to setup random generator
  def setupRandom(self, name, type=None, seed=None, quiet=True):
    rndm = DDG4.Action(self.kernel, 'Geant4Random/' + name)
    if seed:
      rndm.Seed = seed
    if type:
      rndm.Type = type
    rndm.initialize()
    if not quiet:
      rndm.showStatus()
    return rndm

  # Example to show how to configure the Geant4 physics list
  def setupPhysics(self, model):
    phys = self.geant4.setupPhysics(model)
    ph = DDG4.PhysicsList(self.kernel, str('Geant4PhysicsList/Myphysics'))
    # Add bosons to the model (redundant if already implemented by the model)
    ph.addParticleGroup(str('G4BosonConstructor'))
    # Add leptons to the model (redundant if already implemented by the model)
    ph.addParticleGroup(str('G4LeptonConstructor'))
    # Add multiple scattering in the material
    ph.addParticleProcess(str('e[+-]'), str('G4eMultipleScattering'), -1, 1, 1)
    # Add optical physics (RICH dets etc)
    ph.addPhysicsConstructor(str('G4OpticalPhysics'))
    # Interactivity
    ph.enableUI()
    phys.adopt(ph)
    phys.dump()
    return phys

  # No physics list wanted for tests ? See how:
  def noPhysics(self):
    self.geant4.setupPhysics('')
    return self

  def setupDetectors(self):
    logger.info("#  First the tracking detectors")
    seq, act = self.geant4.setupTracker('SiVertexBarrel')
    seq, act = self.geant4.setupTracker('SiVertexEndcap')
    seq, act = self.geant4.setupTracker('SiTrackerBarrel')
    seq, act = self.geant4.setupTracker('SiTrackerEndcap')
    seq, act = self.geant4.setupTracker('SiTrackerForward')
    logger.info("#  Now setup the calorimeters")
    seq, act = self.geant4.setupCalorimeter('EcalBarrel')
    seq, act = self.geant4.setupCalorimeter('EcalEndcap')
    seq, act = self.geant4.setupCalorimeter('HcalBarrel')
    seq, act = self.geant4.setupCalorimeter('HcalEndcap')
    seq, act = self.geant4.setupCalorimeter('HcalPlug')
    seq, act = self.geant4.setupCalorimeter('MuonBarrel')
    seq, act = self.geant4.setupCalorimeter('MuonEndcap')
    seq, act = self.geant4.setupCalorimeter('LumiCal')
    seq, act = self.geant4.setupCalorimeter('BeamCal')
    return self

  # Test the configuration
  def test_config(self, have_geo=True):
    self.kernel.configure()
    if have_geo:
      self.kernel.initialize()
    return self

  # Test runner
  def test_run(self, have_geo=True, have_physics=False, num_events=0):
    self.test_config(have_geo)
    if have_geo:
      self.kernel.NumEvents = num_events
      self.kernel.run()
    self.kernel.terminate()
    logger.info('+++++ All Done....\n\nTEST_PASSED')
    sys.exit(0)
