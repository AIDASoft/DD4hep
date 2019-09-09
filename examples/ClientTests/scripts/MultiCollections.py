from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
from six.moves import range
#
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  batch = False
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  geometry = "file:" + install_dir + "/examples/ClientTests/compact/MultiCollections.xml"
  kernel.setOutputLevel(str('Geant4Converter'), Output.DEBUG)
  kernel.setOutputLevel(str('Gun'), Output.INFO)
  for i in range(len(sys.argv)):
    if sys.argv[i] == '-compact':
      geometry = sys.argv[i + 1]
    elif sys.argv[i] == '-input':
      geometry = sys.argv[i + 1]
    elif sys.argv[i] == '-batch':
      batch = True
    elif sys.argv[i] == 'batch':
      batch = True

  kernel.loadGeometry(str(geometry))
  geant4 = DDG4.Geant4(kernel)
  geant4.printDetectors()
  geant4.setupCshUI()
  if batch:
    kernel.UI = ''

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Configure I/O
  evt_root = geant4.setupROOTOutput('RootOutput', 'Multi_coll_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=True)
  # Setup particle gun
  geant4.setupGun("Gun", particle='pi-', energy=10 * GeV, multiplicity=1)

  # Now the test calorimeter with multiple collections
  seq, act = geant4.setupCalorimeter('TestCal')

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
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
