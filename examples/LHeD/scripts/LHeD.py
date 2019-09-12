from __future__ import absolute_import, unicode_literals
import sys
import logging
import DDG4
from g4units import *

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


class LHeD:
  def __init__(self, tracker='Geant4TrackerCombineAction'):
    self.kernel = DDG4.Kernel()
    self.description = self.kernel.detectorDescription()
    self.geant4 = DDG4.Geant4(self.kernel, tracker=tracker)
    self.kernel.UI = ""
    self.noPhysics()

  def loadGeometry(self):
    import os
    install_dir = os.environ['DD4hepExamplesINSTALL']
    self.kernel.loadGeometry(str("file:" + install_dir + "/examples/LHeD/compact/compact_Lhe_dip_sol_ell.xml"))
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
    ph = DDG4.PhysicsList(self.kernel, 'Geant4PhysicsList/Myphysics')
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
    seq, act = self.geant4.setupTracker('SiTrackerForward')
    seq, act = self.geant4.setupTracker('SiTrackerBarrel')
    seq, act = self.geant4.setupTracker('SiTrackerBackward')
    logger.info("#  Now setup the calorimeters")
    seq, act = self.geant4.setupCalorimeter('EcalBarrel')
    seq, act = self.geant4.setupCalorimeter('EcalEndcap_fwd')
    seq, act = self.geant4.setupCalorimeter('EcalEndcap_bwd')
    seq, act = self.geant4.setupCalorimeter('HcalBarrel')
    seq, act = self.geant4.setupCalorimeter('HcalEndcap_fwd')
    seq, act = self.geant4.setupCalorimeter('HcalEndcap_bwd')
    seq, act = self.geant4.setupCalorimeter('HcalPlug_fwd')
    seq, act = self.geant4.setupCalorimeter('HcalPlug_bwd')
    seq, act = self.geant4.setupCalorimeter('MuonBarrel')
    seq, act = self.geant4.setupCalorimeter('MuonEndcap_fwd1')
    seq, act = self.geant4.setupCalorimeter('MuonEndcap_fwd2')
    seq, act = self.geant4.setupCalorimeter('MuonEndcap_bwd1')
    seq, act = self.geant4.setupCalorimeter('MuonEndcap_bwd2')
    return self

  def test_config(self, have_geo=True):
    self.kernel.configure()
    if have_geo:
      self.kernel.initialize()

  # Test runner
  def test_run(self, have_geo=True, have_physics=False):
    self.test_config(have_geo)
    if have_geo:
      self.kernel.NumEvents = 0
      self.kernel.run()
    self.kernel.terminate()
    logger.info('+++++ All Done....\n\nTEST_PASSED')
    sys.exit(0)
