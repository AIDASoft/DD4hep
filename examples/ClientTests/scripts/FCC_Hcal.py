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
  kernel = DDG4.Kernel()
  kernel.setOutputLevel('Geant4Converter',Output.DEBUG)
  kernel.setOutputLevel('RootOutput',Output.INFO)
  kernel.setOutputLevel('LcioOutput',Output.INFO)
  kernel.setOutputLevel('ShellHandler',Output.DEBUG)
  kernel.setOutputLevel('Gun',Output.INFO)
  kernel.UI = "UI"
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/FCC_HcalBarrel.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
  kernel.printProperties()
  lcdd = kernel.lcdd()
  print '+++  List of sensitive detectors:'
  for i in lcdd.detectors(): 
    print i.second
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
  

  # Configure Run actions: example only!
  """
  run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit')
  run1.Property_int    = 12345
  run1.Property_double = -5e15*keV
  run1.Property_string = 'Startrun: Hello_2'
  print run1.Property_string, run1.Property_double, run1.Property_int
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().add(run1)
  """
  # Configure Event actions
  """
  trk = DDG4.Action(kernel,"Geant4TrackPersistency/MonteCarloTruthHandler")
  mc  = DDG4.Action(kernel,"Geant4MonteCarloRecordManager/MonteCarloRecordManager")
  kernel.registerGlobalAction(trk)
  kernel.registerGlobalAction(mc)
  trk.release()
  mc.release()
  """

  # Configure I/O
  evt_root = DDG4.EventAction(kernel,'Geant4Output2ROOT/RootOutput')
  evt_root.Control = True
  evt_root.Output = "FCC_HCAL_"+time.strftime("%Y-%m-%d_%H-%M")+".root"
  evt_root.enableUI()
  kernel.eventAction().add(evt_root)

  #evt_lcio = DDG4.EventAction(kernel,'Geant4Output2LCIO/LcioOutput')
  #evt_lcio.Output = "simple_lcio"
  #evt_lcio.enableUI()
  #kernel.eventAction().add(evt_lcio)

  # Setup particle gun
  gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGun/Gun")
  gun.energy   = 100*GeV
  gun.particle = 'pi-'
  gun.multiplicity = 1
  gun.position = (0*mm,0*mm,0*mm)
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
  f2 = DDG4.Filter(kernel,'EnergyDepositMinimumCut')
  f2.Cut = 1e-14*keV
  f2.enableUI()
  kernel.registerGlobalFilter(f1)
  kernel.registerGlobalFilter(f2)

  # Now the calorimeters
  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/HcalBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/HcalBarrelHandler','HcalBarrel')
  seq.add(f1)  # ignore geantinos
  #seq.add(f2)  # ignore particles below threshold
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/ContainmentShell')
  act = DDG4.SensitiveAction(kernel,'Geant4EscapeCounter/ShellHandler','ContainmentShell')
  seq.add(act)

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  #phys.transportation = True
  #phys.decays  = True
  phys.enableUI()
  """
  ph = DDG4.PhysicsList(kernel,'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.addParticleConstructor('G4LeptonConstructor')
  ph.addParticleConstructor('G4MesonConstructor')
  ph.addParticleConstructor('G4BaryonConstructor')
  ph.addParticleProcess('e[+-]','G4eMultipleScattering',-1,1,1)
  #ph.addParticleProcess('e[+-]','G4eIonisation',-1,2,2)
  ph.addParticleProcess('mu[+-]','G4MuMultipleScattering',-1,1,1)
  #ph.addParticleProcess('mu[+-]','G4MuIonisation',-1,2,2)
  ph.addParticleProcess('pi[+-]','G4hMultipleScattering',-1,2,2)
  #ph.addParticleProcess('pi[+-]','G4hIonisation',-1,2,2)
  ph.addParticleProcess('pi[+-]','G4hBremsstrahlung',-1,3,3)
  ph.addParticleProcess('proton','G4hMultipleScattering',-1,2,2)
  #ph.addParticleProcess('proton','G4hIonisation',-1,2,2)
  ph.addParticleProcess('proton','G4hBremsstrahlung',-1,3,3)
  ph.enableUI()
  phys.add(ph)
  """

  phys.dump()

  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
