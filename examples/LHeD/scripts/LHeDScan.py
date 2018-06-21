"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
def run():
  import os, sys, logging, DDG4, SystemOfUnits

  logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry("file:"+install_dir+"/examples/LHeD/compact/compact.xml")
  DDG4.Core.setPrintFormat("%-32s %6s %s")
  geant4 = DDG4.Geant4(kernel)
  # Configure UI
  geant4.setupCshUI(ui=None)
  gun = geant4.setupGun("Gun",
                        Standalone=True,
                        particle='geantino',
                        energy=20*SystemOfUnits.GeV,
                        position=(0,0,0),
                        multiplicity=1,
                        isotrop=False )
  scan = DDG4.SteppingAction(kernel,'Geant4MaterialScanner/MaterialScan')
  kernel.steppingAction().adopt(scan)

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  kernel.configure()
  kernel.initialize()
  kernel.NumEvents = 1

  # 3 shots in different directions:
  gun.direction = (0,1,0)
  kernel.run()
  gun.direction = (1,0,0)
  kernel.run()
  gun.direction = (1,1,1)
  kernel.run()

  kernel.terminate()
  logging.info('End of run. Terminating .......')
  logging.info('TEST_PASSED')

if __name__ == "__main__":
  run()
