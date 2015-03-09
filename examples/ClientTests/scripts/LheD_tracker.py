#
#
import os, time, DDG4, sys
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
  lcdd = kernel.lcdd()
  
  install_dir = os.environ['DD4hepINSTALL']
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/LheD_tracker.xml")

  DDG4.importConstants(lcdd,debug=False)
  geant4 = DDG4.Geant4(kernel,tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()

  # Configure UI
  geant4.setupCshUI()
  ##geant4.setupCshUI('csh',True,True)
  ##geant4.setupCshUI('csh',True,True,'vis.mac')
  if len(sys.argv) >= 2 and sys.argv[1] =="batch":
    kernel.UI = ''
  field = geant4.addConfig('Geant4FieldTrackingSetupAction/MagFieldTrackingSetup')
  field.stepper            = "HelixSimpleRunge"
  field.equation           = "Mag_UsualEqRhs"
  field.eps_min            = 5e-05*mm
  field.eps_max            = 0.001*mm
  field.min_chord_step     = 0.01*mm
  field.delta_chord        = 0.25*mm
  field.delta_intersection = 1e-05*mm
  field.delta_one_step     = 0.001*mm
  print '+++++> ',field.name,'-> stepper  = ',field.stepper
  print '+++++> ',field.name,'-> equation = ',field.equation
  print '+++++> ',field.name,'-> eps_min  = ',field.eps_min
  print '+++++> ',field.name,'-> eps_max  = ',field.eps_max
  print '+++++> ',field.name,'-> delta_one_step = ',field.delta_one_step

  # Configure Run actions
  run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit')
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().adopt(run1)

  # Configure Event actions
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.WARNING # Output.WARNING
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.WARNING

  # Configure I/O
  ##evt_lcio = geant4.setupLCIOOutput('LcioOutput','LHeD_tracker_'+time.strftime('%Y-%m-%d_%H-%M'))
  ##evt_lcio.OutputLevel = generator_output_level
  evt_root = geant4.setupROOTOutput('RootOutput','LHeD_tracker_'+time.strftime('%Y-%m-%d_%H-%M'))

  gen = geant4.setupGun("Gun",particle='geantino',energy=20*GeV,position=(0*mm,0*mm,0*cm),multiplicity=3)
  gen.isotrop = False
  gen.direction = (1,0,0)
  gen.OutputLevel = generator_output_level

  #seq,act = geant4.setupTracker('SiVertexBarrel')

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel,'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4Geantino')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  kernel.configure()
  kernel.initialize()

  #DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  print 'End of run. Terminating .......'
  kernel.terminate()

if __name__ == "__main__":
  run()
