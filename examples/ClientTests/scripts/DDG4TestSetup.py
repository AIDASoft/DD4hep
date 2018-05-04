import os, sys, time, logging, DDG4
from DDG4 import OutputLevel as Output
from g4Units import *

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)

"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
class Setup:
  def __init__(self, geometry_file):
    self.kernel = DDG4.Kernel()
    self.kernel.setOutputLevel('Geant4Converter',Output.DEBUG)
    self.kernel.setOutputLevel('Gun',Output.INFO)
    self.kernel.loadGeometry(geometry_file)

    self.geant4 = DDG4.Geant4(self.kernel)
    self.geant4.printDetectors()
    self.geant4.setupCshUI()

  def configure(self):
    # Configure field
    self.field = self.geant4.setupTrackingField(prt=True)
    return self

  def defineOutput(self,output):
    # Configure I/O
    evt_root = self.geant4.setupROOTOutput('RootOutput',output,mc_truth=True)
    return evt_root

  def setupGun(self, name="Gun",particle='pi-',energy=100*GeV,multiplicity=1):
    # Setup particle gun
    self.geant4.setupGun(name,particle=particle,energy=energy,multiplicity=multiplicity)
    return self

  def setupGenerator(self):
    # And handle the simulation particles.
    part = DDG4.GeneratorAction(self.kernel,"Geant4ParticleHandler/ParticleHandler")
    self.kernel.generatorAction().adopt(part)
    part.SaveProcesses = ['conv','Decay']
    part.MinimalKineticEnergy = 1*MeV
    part.OutputLevel = 5 # generator_output_level
    part.enableUI()
    return self

  def setupPhysics(self):
    # Now build the physics list:
    self.phys = self.kernel.physicsList()
    self.phys.extends = 'QGSP_BERT'
    self.phys.enableUI()
    return self

  def run(self):
    # and run
    self.geant4.execute()
    return self

  # Stop the entire excercise
  def terminate(self):
    self.kernel.terminate()
    logging.info('+++++ All Done....\n\nTEST_PASSED')
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
    
