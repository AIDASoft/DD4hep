import os, time, DDG4
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
  kernel.setOutputLevel('RootOutput',Output.INFO)
  kernel.setOutputLevel('ShellHandler',Output.DEBUG)
  kernel.setOutputLevel('Gun',Output.INFO)
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/FCC_HcalBarrel.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")

  simple = DDG4.Simple(kernel)
  simple.printDetectors()
  simple.setupCshUI()

  # Configure I/O
  evt_root = simple.setupROOTOutput('RootOutput','FCC_'+time.strftime('%Y-%m-%d_%H-%M'),mc_truth=False)
  # Setup particle gun
  simple.setupGun("Gun",particle='pi-',energy=100*GeV,multiplicity=1)
  # Now the calorimeters
  seq,act = simple.setupTracker('HcalBarrel')
  seq,act = simple.setupDetector('ContainmentShell','Geant4EscapeCounter')
  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()
  # and run
  simple.execute()

if __name__ == "__main__":
  run()
