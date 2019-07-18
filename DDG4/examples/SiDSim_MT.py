#
#
from __future__ import absolute_import
import os, time, logging, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
#
logging.info("""

   dd4hep simulation example setup DDG4
   in multi-threaded mode using the python configuration

   @author  M.Frank
   @version 1.0

""")


def setupWorker(geant4):
  kernel = geant4.kernel()
  logging.info('#PYTHON: +++ Creating Geant4 worker thread ....')
  logging.info("#PYTHON:  Configure Run actions")
  run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit',shared=True)
  run1.Property_int    = int(12345)
  run1.Property_double = -5e15*keV
  run1.Property_string = 'Startrun: Hello_2'
  logging.info("%s %s %s",run1.Property_string, str(run1.Property_double), str(run1.Property_int))
  run1.enableUI()
  kernel.runAction().adopt(run1)

  logging.info("#PYTHON:  Configure Event actions")
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  logging.info("\n#PYTHON:  Configure I/O\n")
  #evt_lcio = geant4.setupLCIOOutput('LcioOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  #evt_lcio.OutputLevel = Output.ERROR

  evt_root = geant4.setupROOTOutput('RootOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))

  generator_output_level = Output.INFO

  gen = DDG4.GeneratorAction(kernel,"Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)
  #VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  logging.info("#PYTHON:\n#PYTHON:  Generation of isotrope tracks of a given multiplicity with overlay:\n#PYTHON:")
  logging.info("#PYTHON:  First particle generator: pi+")
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropPi+");
  gen.Mask     = 1
  gen.Particle = 'pi+'
  gen.Energy   = 20 * GeV
  gen.Multiplicity = 2
  kernel.generatorAction().adopt(gen)
  logging.info("#PYTHON:  Install vertex smearing for this interaction")
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearPi+");
  gen.Mask   = 1
  gen.Offset = (20*mm, 10*mm, 10*mm, 0*ns)
  gen.Sigma  = (4*mm, 1*mm, 1*mm, 0*ns)
  kernel.generatorAction().adopt(gen)

  logging.info("#PYTHON:  Second particle generator: e-")
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropE-");
  gen.Mask     = 2
  gen.Particle = 'e-'
  gen.Energy   = 15 * GeV
  gen.Multiplicity = 3
  kernel.generatorAction().adopt(gen)
  logging.info("#PYTHON:  Install vertex smearing for this interaction")
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearE-");
  gen.Mask   = 2
  gen.Offset = (-20*mm, -10*mm, -10*mm, 0*ns)
  gen.Sigma  = (12*mm, 8*mm, 8*mm, 0*ns)
  kernel.generatorAction().adopt(gen)
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  logging.info("#PYTHON:  Merge all existing interaction records")
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionMerger/InteractionMerger")
  gen.OutputLevel = 4 #generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  logging.info("#PYTHON:  Finally generate Geant4 primaries")
  gen = DDG4.GeneratorAction(kernel,"Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = 4 #generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  logging.info("#PYTHON:  ....and handle the simulation particles.")
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
  logging.info('#PYTHON: +++ Geant4 worker thread configured successfully....')
  return 1
  
def setupMaster(geant4):
  kernel = geant4.master()
  logging.info('#PYTHON: +++ Setting up master thread for %d workers',int(kernel.NumberOfThreads))
  return 1

def setupSensitives(geant4):
  logging.info("#PYTHON:  Setting up all sensitive detectors")
  geant4.printDetectors()
  logging.info("#PYTHON:  First the tracking detectors")
  seq,act = geant4.setupTracker('SiVertexBarrel')
  seq,act = geant4.setupTracker('SiVertexEndcap')
  seq,act = geant4.setupTracker('SiTrackerBarrel')
  seq,act = geant4.setupTracker('SiTrackerEndcap')
  seq,act = geant4.setupTracker('SiTrackerForward')
  logging.info("#PYTHON:  Now setup the calorimeters")
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
  description = kernel.detectorDescription()
  install_dir = os.environ['DD4hepINSTALL']
  DDG4.Core.setPrintFormat("%-32s %6s %s")
  kernel.loadGeometry("file:"+install_dir+"/DDDetectors/compact/SiD.xml")
  DDG4.importConstants(description)

  kernel.NumberOfThreads = 3
  kernel.RunManagerType  = 'G4MTRunManager'
  geant4 = DDG4.Geant4(kernel,tracker='Geant4TrackerCombineAction')
  logging.info("#  Configure UI")
  geant4.setupCshUI()

  logging.info("#  Geant4 user initialization action")
  geant4.addUserInitialization(worker=setupWorker, worker_args=(geant4,),
                               master=setupMaster, master_args=(geant4,))

  logging.info("#  Configure G4 geometry setup")
  seq,act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")

  logging.info("# Configure G4 sensitive detectors: python setup callback")
  seq,act = geant4.addDetectorConstruction("Geant4PythonDetectorConstruction/SetupSD",
                                           sensitives=setupSensitives,sensitives_args=(geant4,))
  logging.info("# Configure G4 sensitive detectors: atach'em to the sensitive volumes")
  seq,act = geant4.addDetectorConstruction("Geant4DetectorSensitivesConstruction/ConstructSD")
  #                                           allow_threads=True)

  logging.info("#  Configure G4 magnetic field tracking")
  geant4.setupTrackingFieldMT()

  logging.info("#  Setup random generator")
  rndm = DDG4.Action(kernel,'Geant4Random/Random')
  rndm.Seed = 987654321
  rndm.initialize()
  ##rndm.showStatus()

  logging.info("#  Now build the physics list:")
  phys = geant4.setupPhysics('QGSP_BERT')
  phys.dump()

  geant4.run()

if __name__ == "__main__":
  run()
