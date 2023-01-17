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
from __future__ import absolute_import, unicode_literals
import sys
import logging
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV, MeV

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


class Setup:
  def __init__(self, geometry_file, macro=None, vis=None):
    self.kernel = DDG4.Kernel()
    self.kernel.setOutputLevel(str('Geant4Converter'), Output.DEBUG)
    self.kernel.setOutputLevel(str('Gun'), Output.INFO)
    self.description = self.kernel.detectorDescription()
    self.kernel.loadGeometry(str(geometry_file))
    self.geant4 = DDG4.Geant4(self.kernel)
    self.geant4.printDetectors()
    self.ui = self.geant4.setupCshUI(macro=macro, vis=vis)

  def configure(self):
    # Configure field
    self.field = self.geant4.setupTrackingField(prt=True)
    return self

  def defineOutput(self, output):
    # Configure I/O
    evt_write = self.geant4.setupROOTOutput('RootOutput', output, mc_truth=True)
    return evt_write

  def defineEdm4hepOutput(self, output):
    # Configure I/O
    evt_write = self.geant4.setupEDM4hepOutput('Edm4hepOutput', output)
    return evt_write

  def setupGun(self, name="Gun", particle='pi-', energy=100 * GeV, multiplicity=1):
    # Setup particle gun
    return self.geant4.setupGun(name, particle=particle, energy=energy, multiplicity=multiplicity)

  def setupInput(self, spec, mask=1):
    input_action = DDG4.GeneratorAction(self.kernel, "Geant4InputAction/Input")
    input_action.Input = spec
    input_action.MomentumScale = 1.0
    input_action.Mask = mask
    self.geant4.buildInputStage([input_action])

  def setupGenerator(self):
    # And handle the simulation particles.
    part = DDG4.GeneratorAction(self.kernel, "Geant4ParticleHandler/ParticleHandler")
    self.kernel.generatorAction().adopt(part)
    part.SaveProcesses = ['conv', 'Decay']
    part.MinimalKineticEnergy = 1 * MeV
    part.OutputLevel = 5  # generator_output_level
    part.enableUI()
    return part

  def setupPhysics(self, model='QGSP_BERT'):
    # Now build the physics list:
    self.phys = self.kernel.physicsList()
    self.phys.extends = model
    self.phys.enableUI()
    return self

  def run(self, num_events=None):
    # and run
    self.geant4.execute(num_events)
    return self

  # Stop the entire excercise
  def terminate(self):
    self.kernel.terminate()
    logger.info('+++++ All Done....\n\nTEST_PASSED')
    sys.exit(0)

  # Test the configuration
  def test_config(self, have_geo=True):
    self.kernel.configure()
    if have_geo:
      self.kernel.initialize()

  # Test runner
  def test_run(self, have_geo=True, have_physics=False):
    self.test_config(have_geo)
    if have_geo:
      self.kernel.UI = ''
      self.kernel.NumEvents = 0
      self.kernel.run()
    self.terminate()
