import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV, MeV
import logging

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
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.setOutputLevel(str('Geant4Converter'), Output.DEBUG)
  kernel.setOutputLevel(str('Gun'), Output.INFO)
  kernel.detectorDescription().fromXML(str("file:" + install_dir + "/examples/DDCMS/data/dd4hep-ecal.xml"))
  kernel.NumEvents = 5
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()

  batch = False
  test = False
  vis = False
  ui = None
  for i in range(len(sys.argv)):
    arg = sys.argv[i].lower()
    if arg == 'batch':
      batch = True
    elif arg[:4] == '-vis':
      vis = True
    elif arg == 'test':
      test = True
    elif arg == 'numevents':
      kernel.NumEvents = int(sys.argv[i + 1])
  if batch or test:
    geant4.setupCshUI(ui=None, vis=None)
    kernel.UI = 'UI'
  else:
    ui = geant4.setupCshUI(vis=vis)

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'CMSEcal_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=True)
  # Setup particle gun
  generators = []
  generators.append(geant4.setupGun("GunPi-", particle='pi-', energy=300 * GeV,
                                    multiplicity=1, Standalone=False, register=False, Mask=1))
  if not test:
    generators.append(geant4.setupGun("GunE+", particle='e+', energy=100 * GeV,
                                      multiplicity=1, Standalone=False, register=False, Mask=8))
  geant4.buildInputStage(generators)
  # Now setup all tracking detectors
  for i in geant4.description.detectors():
    o = DDG4.DetElement(i.second.ptr())
    sd = geant4.description.sensitiveDetector(o.name())
    if sd.isValid():
      typ = geant4.sensitive_types[sd.type()]
      logger.info('CMSTracker: Configure subdetector %-24s of type %s' % (o.name(), typ,))
      geant4.setupDetector(o.name(), typ)

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
  #
  #
  if ui and vis:
    cmds = []
    cmds.append('/control/verbose 2')
    cmds.append('/run/initialize')
    cmds.append('/vis/open OGL')
    cmds.append('/vis/verbose errors')
    cmds.append('/vis/drawVolume')
    cmds.append('/vis/viewer/set/viewpointThetaPhi 55. 45.')
    cmds.append('/vis/scene/add/axes 0 0 0 3 m')
    ui.Commands = cmds
  #
  # and run
  geant4.execute()


if __name__ == "__main__":
  run()
