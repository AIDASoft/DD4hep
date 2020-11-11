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
import os
import sys
import time
import DDG4
from g4units import GeV, mm, cm
#
"""
   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/Assemblies.xml"))
  #
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  geant4.setupCshUI()
  if len(sys.argv) >= 2 and sys.argv[1] == "batch":
    kernel.UI = ''

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'Assemblies_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=False)
  # Setup particle gun
  geant4.setupGun("Gun", particle='e-', energy=2 * GeV, position=(0.15 * mm, 0.12 * mm, 0.1 * cm), multiplicity=1)
  # First the tracking detectors
  seq, act = geant4.setupTracker('VXD')
  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()

  DDG4.setPrintLevel(DDG4.OutputLevel.DEBUG)
  geant4.execute()


if __name__ == "__main__":
  run()
