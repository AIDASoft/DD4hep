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
import os
import sys
import DDG4
from DDG4 import OutputLevel as Output
from g4units import keV
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
  kernel.loadGeometry(str("file:" + install_dir + "/examples/DDG4/compact/Channeling.xml"))

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
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.INFO

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugMaterials = True
  act.DebugElements = False
  act.DebugVolumes = True
  act.DebugShapes = True
  act.DebugSurfaces = True

  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='gamma', energy=5 * keV, multiplicity=1)
  gun.OutputLevel = generator_output_level

  geant4.setupTracker('ChannelingDevice')

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, 'Channeling')
  ph.addPhysicsConstructor('Geant4ChannelingPhysics')
  ph.enableUI()
  phys.adopt(ph)

  phys.dump()

  geant4.execute()


if __name__ == "__main__":
  run()
