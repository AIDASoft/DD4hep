#
#
from __future__ import absolute_import, unicode_literals
import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import *
import logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  description = kernel.detectorDescription()

  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/LheD_tracker.xml"))

  DDG4.importConstants(description, debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()

  # Configure UI
  geant4.setupCshUI()
  # geant4.setupCshUI('csh',True,True)
  # geant4.setupCshUI('csh',True,True,'vis.mac')
  if len(sys.argv) >= 2 and sys.argv[1] == "batch":
    kernel.UI = ''

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.WARNING  # Output.WARNING
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure I/O
  evt_root = geant4.setupROOTOutput('RootOutput', 'LHeD_tracker_' + time.strftime('%Y-%m-%d_%H-%M'))
  gen = geant4.setupGun("Gun", particle='geantino', energy=20 * GeV, position=(0 * mm, 0 * mm, 0 * cm), multiplicity=3)
  gen.isotrop = False
  gen.direction = (1, 0, 0)
  gen.OutputLevel = Output.WARNING

  #seq,act = geant4.setupTracker('SiVertexBarrel')

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, 'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor(str('G4Geantino'))
  ph.addParticleConstructor(str('G4BosonConstructor'))
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  kernel.configure()
  kernel.initialize()

  # DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  logger.info('End of run. Terminating .......')
  kernel.terminate()


if __name__ == "__main__":
  run()
