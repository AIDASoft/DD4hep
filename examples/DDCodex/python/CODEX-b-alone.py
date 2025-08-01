#
#
import os
import sys
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV, m
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/DDCodex/compact/CODEX-b-alone.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  if len(sys.argv) > 1:
    geant4.setupCshUI(macro=sys.argv[1])
  else:
    geant4.setupCshUI()

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.WARNING
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'CodexB_' + time.strftime('%Y-%m-%d_%H-%M'))

  # Setup particle gun

  # gun = geant4.setupGun("Gun",particle='pi+',
  gun = geant4.setupGun("Gun", particle='mu-',
                        energy=1000 * GeV,
                        multiplicity=1,
                        isotrop=False, Standalone=True,
                        direction=(1, 0, 0),
                        # direction=(0.866025,0,0.5),
                        position='(0,0,12650)')
  # position='(0,0,0)')
  gun.print = True
  """
  gen =  DDG4.GeneratorAction(kernel,"Geant4InputAction/Input")
  # gen.Input = "Geant4EventReaderHepMC|"+
  #             "/afs/cern.ch/work/j/jongho/Project_DD4hep/Test/DD4hep/examples/DDG4/data/hepmc_geant4.dat"
  gen.Input = "Geant4EventReaderHepMC|"+
              "/afs/cern.ch/work/j/jongho/Project_DD4hep/Test/DD4hep/DDG4/examples/MinBias_HepMC.txt"
  gen.MomentumScale = 1.0
  gen.Mask = 1
  geant4.buildInputStage([gen],output_level=Output.DEBUG)
  """

  seq, action = geant4.setupTracker('CODEXb')
  # action.OutputLevel = Output.ERROR
  # seq,action = geant4.setupTracker('Shield')
  # action.OutputLevel = Output.ERROR

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.OutputLevel = Output.INFO
  part.enableUI()
  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 999999. * m  # Something big. All is a tracker
  user.TrackingVolume_Rmax = 999999. * m
  user.enableUI()
  part.adopt(user)

  # Now build the physics list:
  # phys = kernel.physicsList()
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, 'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4LeptonConstructor')
  ph.addParticleConstructor('G4BaryonConstructor')
  ph.addParticleConstructor('G4MesonConstructor')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.enableUI()
  phys.adopt(ph)
  phys.enableUI()
  phys.dump()
  # run
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
