#
#
from __future__ import absolute_import, unicode_literals
import os
import sys
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV
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
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/GdmlDetector.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  if len(sys.argv) > 1:
    geant4.setupCshUI(macro=sys.argv[1])
  else:
    geant4.setupCshUI()

  # Configure field
  geant4.setupTrackingField(prt=True)

  generator_output_level = Output.INFO

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugVolumes = True
  act.DebugShapes = True
  act.DebugPlacements = True
  act.DumpHierarchy = True

  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='mu-', energy=20 * GeV, multiplicity=1)
  gun.OutputLevel = generator_output_level

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  phys.dump()

  geant4.execute()


if __name__ == "__main__":
  run()
