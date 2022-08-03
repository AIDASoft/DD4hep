#
#
from __future__ import absolute_import, unicode_literals
import os
import time
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
  args = DDG4.CommandLine()
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/SiliconBlock.xml"))
  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  # =======================================================================================
  # ===> This is actually the ONLY difference to ClientTests/scripts/SiliconBlock.py
  # =======================================================================================
  geant4 = DDG4.Geant4(kernel, tracker='MyTrackerSDAction')
  geant4.printDetectors()

  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro)
  else:
    ui = geant4.setupCshUI()
  if args.batch:
    ui.Commands = ['/run/beamOn ' + str(args.events), '/ddg4/UI/terminate']

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.WARNING
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'MySD_' + time.strftime('%Y-%m-%d_%H-%M'), mc_truth=False)
  # Setup particle gun
  geant4.setupGun("Gun", particle='mu-', energy=5 * GeV, multiplicity=1, Standalone=True, position=(0, 0, 0))
  seq, act = geant4.setupTracker('SiliconBlockUpper')
  act.OutputLevel = Output.INFO
  seq, act = geant4.setupTracker('SiliconBlockDown')
  act.OutputLevel = Output.INFO
  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()
  # run
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
