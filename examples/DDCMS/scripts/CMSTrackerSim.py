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
  kernel.loadGeometry("file:"+install_dir+"/examples/DDCMS/data/cms_tracker.xml")
  kernel.detectorDescription().fromXML("file:"+install_dir+"/examples/DDCMS/data/dd4hep-config.xml");
  kernel.NumEvents = 5
  geant4 = DDG4.Geant4(kernel)
  geant4.printDetectors()
  geant4.setupCshUI()
  if len(sys.argv) >= 2 and sys.argv[1] =="batch":
    kernel.UI = ''

  # Configure field
  field = geant4.setupTrackingField(prt=True)
  # Configure I/O
  evt_root = geant4.setupROOTOutput('RootOutput','CMSTracker_'+time.strftime('%Y-%m-%d_%H-%M'),mc_truth=True)
  # Setup particle gun
  geant4.setupGun("Gun",particle='pi-',energy=100*GeV,multiplicity=1)
  # Now setup all tracking detectors
  for i in geant4.description.detectors():
    o = DDG4.DetElement(i.second.ptr())
    sd = geant4.description.sensitiveDetector(o.name())
    if sd.isValid():
      type = geant4.sensitive_types[sd.type()]
      print 'CMSTracker: Configure subdetector %-24s of type %s'%(o.name(),type,)
      geant4.setupDetector(o.name(),type)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['conv','Decay']
  part.MinimalKineticEnergy = 1*MeV
  part.OutputLevel = 5 # generator_output_level
  part.enableUI()

  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  # and run
  geant4.execute()

if __name__ == "__main__":
  run()
