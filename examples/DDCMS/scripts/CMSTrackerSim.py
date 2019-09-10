from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
import logging
from ddsix.moves import range

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

#
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepINSTALL']
  kernel.setOutputLevel(str('Geant4Converter'), Output.DEBUG)
  kernel.setOutputLevel(str('Gun'), Output.INFO)
  kernel.detectorDescription().fromXML(str("file:" + install_dir + "/examples/DDCMS/data/dd4hep-config.xml"))
  kernel.NumEvents = 5
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  geant4.setupCshUI()
  batch = False
  test = False
  for i in range(len(sys.argv)):
    arg = sys.argv[i].lower()
    if arg == 'batch':
      batch = True
    elif arg == 'test':
      test = True
    elif arg == 'numevents':
      kernel.NumEvents = int(sys.argv[i + 1])
  if batch or test:
    kernel.UI = ''

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Configure I/O
  evt_root = geant4.setupROOTOutput('RootOutput', 'CMSTracker_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=True)
  # Setup particle gun
  generators = []
  generators.append(geant4.setupGun("GunPi-", particle='pi-', energy=300 * GeV,
                                    multiplicity=1, Standalone=False, register=False, Mask=1))
  if not test:
    generators.append(geant4.setupGun("GunPi+", particle='pi+', energy=300 * GeV,
                                      multiplicity=1, Standalone=False, register=False, Mask=2))
    generators.append(geant4.setupGun("GunE-", particle='e-', energy=100 * GeV,
                                      multiplicity=1, Standalone=False, register=False, Mask=4))
    generators.append(geant4.setupGun("GunE+", particle='e+', energy=100 * GeV,
                                      multiplicity=1, Standalone=False, register=False, Mask=8))
  geant4.buildInputStage(generators)
  # Now setup all tracking detectors
  for i in geant4.description.detectors():
    o = DDG4.DetElement(i.second.ptr())
    sd = geant4.description.sensitiveDetector(o.name())
    if sd.isValid():
      type = geant4.sensitive_types[sd.type()]
      logger.info('CMSTracker: Configure subdetector %-24s of type %s' % (o.name(), type,))
      geant4.setupDetector(o.name(), type)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['conv', 'Decay']
  part.MinimalKineticEnergy = 1 * MeV
  part.OutputLevel = 5  # generator_output_level
  part.enableUI()

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  # and run
  geant4.execute()


if __name__ == "__main__":
  run()
