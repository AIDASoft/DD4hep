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
#
from __future__ import absolute_import, unicode_literals
import os
import sys
import DDG4
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/TrackingRegion.xml"))
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  # Configure field
  # field = geant4.setupTrackingField(prt=True)
  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugVolumes = True
  act.DebugPlacements = True
  geant4.setupTracker('SiliconBlock')
  kernel.configure()
  kernel.initialize()
  kernel.terminate()
  sys.exit(0)


if __name__ == "__main__":
  run()
