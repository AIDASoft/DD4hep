import os, time, DDG4
from SystemOfUnits import *
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
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/Assemblies.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
  #
  simple = DDG4.Simple(kernel,tracker='Geant4TrackerCombineAction')
  simple.printDetectors()
  # Configure UI
  simple.setupCshUI()
  # Configure I/O
  simple.setupROOTOutput('RootOutput','Assemblies_'+time.strftime('%Y-%m-%d_%H-%M'),mc_truth=False)
  # Setup particle gun
  simple.setupGun("Gun",particle='e-',energy=2*GeV,position=(0.15*mm,0.12*mm,0.1*cm),multiplicity=1)
  # First the tracking detectors
  seq,act = simple.setupTracker('VXD')
  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()

  DDG4.setPrintLevel(DDG4.OutputLevel.DEBUG)
  simple.execute()

if __name__ == "__main__":
  run()
