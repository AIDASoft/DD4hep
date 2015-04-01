import os, time, DDG4, sys
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   DD4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir+'/examples/DDG4/examples';
  kernel.setOutputLevel('Geant4Converter',Output.DEBUG)
  kernel.setOutputLevel('Gun',Output.INFO)
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/NestedDetectors.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")

  simple = DDG4.Simple(kernel)
  simple.printDetectors()
  simple.setupCshUI()
  if len(sys.argv) >= 2 and sys.argv[1] =="batch":
    kernel.UI = ''

  # Configure I/O
  evt_root = simple.setupROOTOutput('RootOutput','Nested_'+time.strftime('%Y-%m-%d_%H-%M'),mc_truth=True)
  # Setup particle gun
  simple.setupGun("Gun",particle='pi-',energy=100*GeV,multiplicity=1)
  # Now the calorimeters
  seq,act = simple.setupTracker('SiTrackerBarrel')
  seq,act = simple.setupTracker('SiVertexBarrel')
  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['conv','Decay']
  part.MinimalKineticEnergy = 1*MeV
  part.enableUI()

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()
  # and run
  simple.execute()

if __name__ == "__main__":
  run()
