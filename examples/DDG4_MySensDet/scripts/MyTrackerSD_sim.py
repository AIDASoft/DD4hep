#
#
import os, sys, time, DDG4, dd4hep
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/SiliconBlock.xml")
  DDG4.importConstants(kernel.detectorDescription(),debug=False)
  # =======================================================================================
  # ===> This is actually the ONLY difference to ClientTests/scripts/SiliconBlock.py
  # =======================================================================================
  geant4 = DDG4.Geant4(kernel,tracker='MyTrackerSDAction')

  geant4.printDetectors()
  kernel.NumEvents = 5
  kernel.UI = ''

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.WARNING
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure I/O
  evt_root = geant4.setupROOTOutput('RootOutput','MySD_'+time.strftime('%Y-%m-%d_%H-%M'),mc_truth=False)
  # Setup particle gun
  gun = geant4.setupGun("Gun",particle='mu-',energy=5*GeV,multiplicity=1,Standalone=True,position=(0,0,0))
  geant4.setupTracker('SiliconBlockUpper')
  geant4.setupTracker('SiliconBlockDown')
  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()
  # run
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
