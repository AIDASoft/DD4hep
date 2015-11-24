#
#
import os, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
print \
"""

   DD4hep simulation example setup DDG4
   in multi-threaded mode using the python configuration

   @author  M.Frank
   @version 1.0

"""


def setupWorker(geant4):
  kernel = geant4.kernel()
  print '#PYTHON: +++ Creating Geant4 worker thread ....'
  print "#PYTHON:  Configure Run actions"
  run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit')
  run1.Property_int    = 12345
  run1.Property_double = -5e15*keV
  run1.Property_string = 'Startrun: Hello_2'
  print run1.Property_string, run1.Property_double, run1.Property_int
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().adopt(run1)

  print "#PYTHON:  Configure Event actions"
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  print "\n#PYTHON:  Configure I/O\n"
  evt_lcio = geant4.setupLCIOOutput('LcioOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  evt_lcio.OutputLevel = Output.ERROR

  evt_root = geant4.setupROOTOutput('RootOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))

  generator_output_level = Output.INFO

  gen = DDG4.GeneratorAction(kernel,"Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)
  #VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  print "#PYTHON:\n#PYTHON:  Generation of isotrope tracks of a given multiplicity with overlay:\n#PYTHON:"
  print "#PYTHON:  First particle generator: pi+"
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropPi+");
  gen.Mask     = 1
  gen.Particle = 'pi+'
  gen.Energy   = 100 * GeV
  gen.Multiplicity = 2
  kernel.generatorAction().adopt(gen)
  print "#PYTHON:  Install vertex smearing for this interaction"
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearPi+");
  gen.Mask   = 1
  gen.Offset = (20*mm, 10*mm, 10*mm, 0*ns)
  gen.Sigma  = (4*mm, 1*mm, 1*mm, 0*ns)
  kernel.generatorAction().adopt(gen)

  print "#PYTHON:  Second particle generator: e-"
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropE-");
  gen.Mask     = 2
  gen.Particle = 'e-'
  gen.Energy   = 25 * GeV
  gen.Multiplicity = 3
  kernel.generatorAction().adopt(gen)
  print "#PYTHON:  Install vertex smearing for this interaction"
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearE-");
  gen.Mask   = 2
  gen.Offset = (-20*mm, -10*mm, -10*mm, 0*ns)
  gen.Sigma  = (12*mm, 8*mm, 8*mm, 0*ns)
  kernel.generatorAction().adopt(gen)
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  print "#PYTHON:  Merge all existing interaction records"
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionMerger/InteractionMerger")
  gen.OutputLevel = 4 #generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  print "#PYTHON:  Finally generate Geant4 primaries"
  gen = DDG4.GeneratorAction(kernel,"Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = 4 #generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  print "#PYTHON:  ....and handle the simulation particles."
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  #part.SaveProcesses = ['conv','Decay']
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100*MeV
  part.OutputLevel = 5 # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = DDG4.EcalEndcap_zmin
  user.TrackingVolume_Rmax = DDG4.EcalBarrel_rmin
  user.enableUI()
  part.adopt(user)
  print '#PYTHON: +++ Geant4 worker thread configured successfully....'
  return 1
  
def setupMaster(geant4):
  kernel = geant4.master()
  print '#PYTHON: +++ Setting up master thread for ',kernel.NumberOfThreads,' workers.'
  return 1

def setupSensitives(geant4):
  print "#PYTHON:  Setting up all sensitive detectors"
  geant4.printDetectors()
  print "#PYTHON:  First the tracking detectors"
  seq,act = geant4.setupTracker('SiVertexBarrel')
  seq,act = geant4.setupTracker('SiVertexEndcap')
  seq,act = geant4.setupTracker('SiTrackerBarrel')
  seq,act = geant4.setupTracker('SiTrackerEndcap')
  seq,act = geant4.setupTracker('SiTrackerForward')
  print "#PYTHON:  Now setup the calorimeters"
  seq,act = geant4.setupCalorimeter('EcalBarrel')
  seq,act = geant4.setupCalorimeter('EcalEndcap')
  seq,act = geant4.setupCalorimeter('HcalBarrel')
  seq,act = geant4.setupCalorimeter('HcalEndcap')
  seq,act = geant4.setupCalorimeter('HcalPlug')
  seq,act = geant4.setupCalorimeter('MuonBarrel')
  seq,act = geant4.setupCalorimeter('MuonEndcap')
  seq,act = geant4.setupCalorimeter('LumiCal')
  seq,act = geant4.setupCalorimeter('BeamCal')
  return 1

def run():
  kernel = DDG4.Kernel()
  lcdd = kernel.lcdd()
  install_dir = os.environ['DD4hepINSTALL']
  DDG4.Core.setPrintFormat("%-32s %6s %s")
  kernel.loadGeometry("file:"+install_dir+"/DDDetectors/compact/SiD.xml")
  DDG4.importConstants(lcdd)

  kernel.NumberOfThreads = 3
  geant4 = DDG4.Geant4(kernel,tracker='Geant4TrackerCombineAction')
  print "#  Configure UI"
  geant4.setupCshUI()

  print "#  Geant4 user initialization action"
  geant4.addUserInitialization(worker=setupWorker, worker_args=(geant4,),
                               master=setupMaster,master_args=(geant4,))

  print "#  Configure G4 geometry setup"
  seq,act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")

  print "# Configure G4 sensitive detectors: python setup callback"
  seq,act = geant4.addDetectorConstruction("Geant4PythonDetectorConstruction/SetupSD",
                                           sensitives=setupSensitives,sensitives_args=(geant4,))
  print "# Configure G4 sensitive detectors: atach'em to the sensitive volumes"
  seq,act = geant4.addDetectorConstruction("Geant4DetectorSensitivesConstruction/ConstructSD")
  #                                           allow_threads=True)

  print "#  Configure G4 magnetic field tracking"
  seq,field = geant4.addDetectorConstruction("Geant4FieldTrackingConstruction/MagFieldTrackingSetup")
  field.stepper            = "HelixGeant4Runge"
  field.equation           = "Mag_UsualEqRhs"
  field.eps_min            = 5e-05 * mm
  field.eps_max            = 0.001 * mm
  field.min_chord_step     = 0.01 * mm
  field.delta_chord        = 0.25 * mm
  field.delta_intersection = 1e-05 * mm
  field.delta_one_step     = 0.001 * mm
  print '+++++> ',field.name,'-> stepper  = ',field.stepper
  print '+++++> ',field.name,'-> equation = ',field.equation
  print '+++++> ',field.name,'-> eps_min  = ',field.eps_min
  print '+++++> ',field.name,'-> eps_max  = ',field.eps_max
  print '+++++> ',field.name,'-> delta_one_step = ',field.delta_one_step

  print "#  Setup random generator"
  rndm = DDG4.Action(kernel,'Geant4Random/Random')
  rndm.Seed = 987654321
  rndm.initialize()
  ##rndm.showStatus()

  print "#  Now build the physics list:"
  phys = geant4.setupPhysics('QGSP_BERT')
  phys.dump()

  geant4.run()

if __name__ == "__main__":
  run()
