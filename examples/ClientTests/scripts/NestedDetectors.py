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
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV, MeV
#
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  args = DDG4.CommandLine()
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.setOutputLevel(str('Geant4Converter'), Output.DEBUG)
  kernel.setOutputLevel(str('Gun'), Output.INFO)
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/NestedDetectors.xml"))

  geant4 = DDG4.Geant4(kernel)
  geant4.printDetectors()
  geant4.setupCshUI()
  if args.batch:
    kernel.UI = ''

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'Nested_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=True)
  # Setup particle gun
  geant4.setupGun("Gun", particle='pi-', energy=100 * GeV, multiplicity=1)
  # Now the calorimeters
  seq, act = geant4.setupTracker('SiTrackerBarrel')
  seq, act = geant4.setupTracker('SiVertexBarrel')
  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['conv', 'Decay']
  part.MinimalKineticEnergy = 1 * MeV
  part.enableUI()

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()
  # and run
  geant4.execute()


if __name__ == "__main__":
  run()
