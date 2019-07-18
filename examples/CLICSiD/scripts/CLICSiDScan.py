"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import
import logging

logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

def run():
  import os, sys, DDG4, CLICSid, g4units

  sid = CLICSid.CLICSid()
  sid.loadGeometry()
  DDG4.Core.setPrintFormat("%-32s %6s %s")
  geant4 = sid.geant4
  # Configure UI
  sid.geant4.setupCshUI(ui=None)
  gun = sid.geant4.setupGun("Gun",
                        Standalone=True,
                        particle='geantino',
                        energy=20*g4units.GeV,
                        position=(0,0,0),
                        multiplicity=1,
                        isotrop=False )
  scan = DDG4.SteppingAction(sid.kernel,'Geant4MaterialScanner/MaterialScan')
  sid.kernel.steppingAction().adopt(scan)

  # Now build the physics list:
  phys = sid.setupPhysics('QGSP_BERT')
  sid.test_config()
  sid.kernel.NumEvents = 1

  # 3 shots in different directions:
  gun.direction = (0,1,0)
  sid.kernel.run()
  gun.direction = (1,0,0)
  sid.kernel.run()
  gun.direction = (1,1,1)
  sid.kernel.run()

  sid.kernel.terminate()
  logger.info('End of run. Terminating .......')
  logger.info('TEST_PASSED')

if __name__ == "__main__":
  run()
