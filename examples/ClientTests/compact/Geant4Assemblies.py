#
#
import os, DDG4
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
  kernel.UI = "UI"
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/Assemblies.xml")
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

  evt1 = DDG4.EventAction(kernel,'Geant4TestEventAction/UserEvent_1')
  evt1.Property_int=01234
  evt1.Property_double=1e11
  evt1.Property_string='Hello_1'
  evt1.enableUI()

  kernel.eventAction().add(evt1)

  # Configure I/O
  evt_root = DDG4.EventAction(kernel,'Geant4Output2ROOT/RootOutput')
  evt_root.Control = True
  evt_root.Output = "simple.root"
  evt_root.enableUI()
  kernel.eventAction().add(evt_root)

  # Setup particle gun
  gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGun/Gun")
  gun.energy   = 0.5*GeV
  gun.particle = 'e-'
  gun.multiplicity = 1
  gun.position = (0.15*mm,0.12*mm,0.1*cm)
  gun.enableUI()
  kernel.generatorAction().add(gun)

  # Setup global filters fur use in sensntive detectors
  f1 = DDG4.Filter(kernel,'GeantinoRejectFilter/GeantinoRejector')
  f4 = DDG4.Filter(kernel,'EnergyDepositMinimumCut')
  f4.Cut = 0.1*MeV
  f4.enableUI()
  kernel.registerGlobalFilter(f1)
  kernel.registerGlobalFilter(f4)

  # First the tracking detectors
  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/VXD')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/VXDHandler','VXD')
  seq.add(act)
  seq.add(f1)
  seq.add(f4)
  act.add(f1)

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
