#
#
import os, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   DD4hep example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""
def run():
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir+'/examples/DDG4/examples';
  DDG4.setPrintLevel(Output.DEBUG)
  kernel = DDG4.Kernel()
  kernel.UI = "UI"
  kernel.loadGeometry("file:"+install_dir+"/examples/CLICSiD/compact/compact.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")

  lcdd = kernel.lcdd()
  print '+++   List of sensitive detectors:'
  for i in lcdd.detectors(): 
    o = DDG4.DetElement(i.second)
    sd = lcdd.sensitiveDetector(o.name())
    if sd.isValid():
      print '+++  %-32s type:%s'%(o.name(), sd.type(), )

  # Configure UI
  ui = DDG4.Action(kernel,"Geant4UIManager/UI")
  ui.HaveVIS = True
  ui.HaveUI = True
  ui.SessionType = 'csh'
  kernel.registerGlobalAction(ui)
  

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
  trk = DDG4.Action(kernel,"Geant4TrackPersistency/MonteCarloTruthHandler")
  kernel.registerGlobalAction(trk)
  trk.release()
  mc  = DDG4.Action(kernel,"Geant4MonteCarloRecordManager/MonteCarloRecordManager")
  kernel.registerGlobalAction(mc)
  mc.release()
  """
  # Configure I/O
  evt_root = DDG4.EventAction(kernel,'Geant4Output2ROOT/RootOutput')
  evt_root.Control = True
  evt_root.Output = "CLICSiD_"+time.strftime("%Y-%m-%d_%H-%M")+".root"
  evt_root.enableUI()
  kernel.eventAction().add(evt_root)

  #evt_lcio = DDG4.EventAction(kernel,'Geant4Output2LCIO/LcioOutput')
  #evt_lcio.Output = "simple_lcio"
  #evt_lcio.enableUI()
  #kernel.eventAction().add(evt_lcio)

  gen = DDG4.GeneratorAction(kernel,"Geant4TestGeneratorAction/Generate")
  kernel.generatorAction().add(gen)

  # Setup particle gun
  gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGun/Gun")
  gun.energy   = 100*GeV
  gun.particle = 'pi-'
  gun.multiplicity = 1
  gun.position = (0*mm,0*mm,0*cm)
  gun.isotrop = True
  gun.enableUI()
  kernel.generatorAction().add(gun)
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
  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/SiVertexBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/SiVertexBarrelHandler','SiVertexBarrel')
  seq.add(act)
  seq.add(f1)
  seq.add(f4)
  act.add(f1)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/SiVertexEndcap')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/SiVertexEndcapHandler','SiVertexEndcap')
  seq.add(act)
  seq.add(f1)
  seq.add(f4)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/SiTrackerBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/SiTrackerBarrelHandler','SiTrackerBarrel')
  seq.add(act)
  seq.add(f1)
  seq.add(f4)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/SiTrackerEndcap')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/SiTrackerEndcapHandler','SiTrackerEndcap')
  #act.OutputLevel = Output.INFO
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/SiTrackerForward')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/SiTrackerForwardHandler','SiTrackerForward')
  seq.add(act)

  # Now the calorimeters
  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/EcalBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/EcalBarrelHandler','EcalBarrel')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/EcalEndcap')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/EcalEndCapHandler','EcalEndcap')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/HcalBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/HcalBarrelHandler','HcalBarrel')
  act.adoptFilter(kernel.globalFilter('OpticalPhotonRejector'))
  seq.add(act)

  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/HcalBarrelHandler','HcalBarrel')
  act.adoptFilter(kernel.globalFilter('OpticalPhotonSelector'))
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/HcalEndcap')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/HcalEndcapHandler','HcalEndcap')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/HcalPlug')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/HcalPlugHandler','HcalPlug')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/MuonBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/MuonBarrelHandler','MuonBarrel')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/MuonEndcap')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/MuonEndcapHandler','MuonEndcap')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/LumiCal')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/LumiCalHandler','LumiCal')
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/BeamCal')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/BeamCalHandler','BeamCal')
  seq.add(act)

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'FTFP_BERT'
  #phys.transportation = True
  phys.decays  = True
  phys.enableUI()

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

  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
