import os, sys, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
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
  kernel.setOutputLevel('Geant4Converter',Output.DEBUG)
  kernel.setOutputLevel('Gun',Output.INFO)
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/MiniTel.xml")

  geant4 = DDG4.Geant4(kernel)
  geant4.printDetectors()
  geant4.setupCshUI()
  if len(sys.argv) >= 2 and sys.argv[1] =="batch":
    kernel.NumEvents = 200
    kernel.UI = ''

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Setup particle gun
  geant4.setupGun("Gun",particle='pi-',energy=100*GeV,multiplicity=1)
  # Now the calorimeters
  seq,act = geant4.setupTracker('MyLHCBdetector1')
  seq,act = geant4.setupTracker('MyLHCBdetector2')
  seq,act = geant4.setupTracker('MyLHCBdetector3')
  seq,act = geant4.setupTracker('MyLHCBdetector4')
  act.OutputLevel = 4
  seq,act = geant4.setupTracker('MyLHCBdetector5')
  seq,act = geant4.setupTracker('MyLHCBdetector6')
  seq,act = geant4.setupTracker('MyLHCBdetector7')
  seq,act = geant4.setupTracker('MyLHCBdetector8')
  seq,act = geant4.setupTracker('MyLHCBdetector9')
  seq,act = geant4.setupTracker('MyLHCBdetector10')

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['conv','Decay']
  part.MinimalKineticEnergy = 1*MeV
  part.OutputLevel = 5 # generator_output_level
  part.enableUI()

  hit_tuple = DDG4.EventAction(kernel,'HitTupleAction/MiniTelTuple',True)
  hit_tuple.OutputFile = 'MiniTel_EnergyDeposits_'+time.strftime('%Y-%m-%d_%H-%M')+'.root'
  hit_tuple.Collections = ['*']
  kernel.eventAction().add(hit_tuple)


  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  # and run
  geant4.execute()

if __name__ == "__main__":
  run()
