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

  # First the tracking detectors
  seq = DDG4.SensitiveSequence(kernel,'Geant4SensDetActionSequence/VXD')
  act = DDG4.SensitiveAction(kernel,'Geant4SimpleTrackerAction/VXDHandler','VXD')

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()

  phys.dump()

  DDG4.setPrintLevel(DDG4.OutputLevel.DEBUG)
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
