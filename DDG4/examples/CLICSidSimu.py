#
#
import os, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   DD4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir+'/examples/DDG4/examples';
  kernel.loadGeometry("file:"+install_dir+"/examples/CLICSiD/compact/compact.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")

  simple = DDG4.Simple(kernel)
  simple.printDetectors()
  # Configure UI
  simple.setupCshUI()

  # Configure Run actions
  run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit')
  run1.Property_int    = 12345
  run1.Property_double = -5e15*keV
  run1.Property_string = 'Startrun: Hello_2'
  print run1.Property_string, run1.Property_double, run1.Property_int
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().add(run1)

  # Configure Event actions
  evt2 = DDG4.EventAction(kernel,'Geant4TestEventAction/UserEvent_2')
  evt2.Property_int    = 123454321
  evt2.Property_double = 5e15*GeV
  evt2.Property_string = 'Hello_2 from the python setup'
  evt2.enableUI()
  kernel.registerGlobalAction(evt2)

  evt1 = DDG4.EventAction(kernel,'Geant4TestEventAction/UserEvent_1')
  evt1.Property_int=01234
  evt1.Property_double=1e11
  evt1.Property_string='Hello_1'
  evt1.enableUI()

  kernel.eventAction().add(evt1)
  kernel.eventAction().add(evt2)

  """
  mc  = DDG4.Action(kernel,"Geant4MonteCarloRecordManager/MonteCarloRecordManager")
  kernel.registerGlobalAction(mc)
  mc.release()
  """
  # Configure I/O
  evt_root = simple.setupROOTOutput('RootOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  evt_lcio = simple.setupLCIOOutput('LcioOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))

  gen = DDG4.GeneratorAction(kernel,"Geant4TestGeneratorAction/Generate")
  kernel.generatorAction().add(gen)

  # Setup particle gun
  gun = simple.setupGun('Gun','pi-',100*GeV,True)

  trk = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().add(trk)
  trk.SaveProcesses = ['conv','Decay']
  trk.enableUI()
  """
  """

  """
  rdr = DDG4.GeneratorAction(kernel,"LcioGeneratorAction/Reader")
  rdr.zSpread = 0.0
  rdr.lorentzAngle = 0.0
  rdr.OutputLevel = DDG4.OutputLevel.INFO
  rdr.Input = "LcioEventReader|test.data"
  rdr.enableUI()
  kernel.generatorAction().add(rdr)
  """

  # Setup global filters fur use in sensntive detectors
  f1 = DDG4.Filter(kernel,'GeantinoRejectFilter/GeantinoRejector')
  f2 = DDG4.Filter(kernel,'ParticleRejectFilter/OpticalPhotonRejector')
  f2.particle = 'opticalphoton'
  f3 = DDG4.Filter(kernel,'ParticleSelectFilter/OpticalPhotonSelector') 
  f3.particle = 'opticalphoton'
  f4 = DDG4.Filter(kernel,'EnergyDepositMinimumCut')
  f4.Cut = 10*MeV
  f4.enableUI()
  kernel.registerGlobalFilter(f1)
  kernel.registerGlobalFilter(f2)
  kernel.registerGlobalFilter(f3)
  kernel.registerGlobalFilter(f4)

  # First the tracking detectors
  seq,act = simple.setupTracker('SiVertexBarrel')
  seq.add(f1)
  #seq.add(f4)
  act.add(f1)

  seq,act = simple.setupTracker('SiVertexEndcap')
  seq.add(f1)
  #seq.add(f4)

  seq,act = simple.setupTracker('SiTrackerBarrel')
  seq,act = simple.setupTracker('SiTrackerEndcap')
  seq,act = simple.setupTracker('SiTrackerForward')
  # Now the calorimeters
  seq,act = simple.setupCalorimeter('EcalBarrel')
  seq,act = simple.setupCalorimeter('EcalEndcap')
  seq,act = simple.setupCalorimeter('HcalBarrel')
  seq,act = simple.setupCalorimeter('HcalEndcap')
  seq,act = simple.setupCalorimeter('HcalPlug')
  seq,act = simple.setupCalorimeter('MuonBarrel')
  seq,act = simple.setupCalorimeter('MuonEndcap')
  seq,act = simple.setupCalorimeter('LumiCal')
  seq,act = simple.setupCalorimeter('BeamCal')

  # Now build the physics list:
  phys = simple.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel,'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.addParticleConstructor('G4LeptonConstructor')
  ph.addParticleProcess('e[+-]','G4eMultipleScattering',-1,1,1)
  ph.addPhysicsConstructor('G4OpticalPhysics')
  ph.enableUI()
  phys.add(ph)

  phys.dump()

  kernel.configure()
  kernel.initialize()

  #DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
