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
  
  # Configure I/O
  evt_root = DDG4.EventAction(kernel,'Geant4Output2ROOT/RootOutput')
  evt_root.Control = True
  evt_root.Output = "FCC_HCAL_"+time.strftime("%Y-%m-%d_%H-%M")+".root"
  evt_root.enableUI()
  kernel.eventAction().add(evt_root)

  # Setup particle gun
  gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGun/Gun")
  gun.energy   = 100*GeV
  gun.particle = 'pi-'
  gun.multiplicity = 1
  gun.position = (0*mm,0*mm,0*mm)
  gun.isotrop = True
  gun.enableUI()
  kernel.generatorAction().add(gun)

  # Setup global filters fur use in sensntive detectors
  f1 = DDG4.Filter(kernel,'GeantinoRejectFilter/GeantinoRejector')
  kernel.registerGlobalFilter(f1)

  # Now the calorimeters
  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/HcalBarrel')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleCalorimeterAction/HcalBarrelHandler','HcalBarrel')
  seq.add(f1)  # ignore geantinos
  seq.add(act)

  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/ContainmentShell')
  act = DDG4.SensitiveAction(kernel,'Geant4EscapeCounter/ShellHandler','ContainmentShell')
  seq.add(act)

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()

  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
