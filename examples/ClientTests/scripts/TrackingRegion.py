#
#
from __future__ import absolute_import
import os, sys, time, DDG4
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/TrackingRegion.xml")
  geant4 = DDG4.Geant4(kernel,tracker='Geant4TrackerCombineAction')
  # Configure field
  ##field = geant4.setupTrackingField(prt=True)
  # Configure G4 geometry setup
  seq,act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugVolumes    = True
  act.DebugPlacements = True
  geant4.setupTracker('SiliconBlock')
  kernel.configure()
  kernel.initialize()
  kernel.terminate()
  sys.exit(0)

if __name__ == "__main__":
  run()
