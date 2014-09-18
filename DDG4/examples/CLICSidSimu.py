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
  kernel.runAction().adopt(run1)

  # Configure Event actions
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure I/O
  evt_root = simple.setupROOTOutput('RootOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  evt_lcio = simple.setupLCIOOutput('LcioOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  evt_lcio.OutputLevel = Output.ERROR


  generator_output_level = Output.INFO

  gen = DDG4.GeneratorAction(kernel,"Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)

  #VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of isotrope tracks using the DDG4 partcle gun:

  # Setup particle gun
  gun = simple.setupGun('Gun','pi-',energy=10*GeV,isotrop=True,multiplicity=3)
  gun.OutputLevel = 5 # generator_output_level
  gun.Mask = 1
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  """

  #VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of isotrope tracks of a given multiplicity with overlay:

  # First particle generator: pi+
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropPi+");
  gen.Particle = 'pi+'
  gen.Energy = 100 * GeV
  gen.Multiplicity = 2
  gen.Mask = 1
  kernel.generatorAction().adopt(gen)
  # Install vertex smearing for this interaction
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearPi+");
  gen.Mask = 1
  gen.Offset = (20*mm, 10*mm, 10*mm, 0*ns)
  gen.Sigma = (4*mm, 1*mm, 1*mm, 0*ns)
  kernel.generatorAction().adopt(gen)

  # Second particle generator: e-
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropE-");
  gen.Particle = 'e-'
  gen.Energy = 25 * GeV
  gen.Multiplicity = 3
  gen.Mask = 2
  kernel.generatorAction().adopt(gen)
  # Install vertex smearing for this interaction
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearE-");
  gen.Mask = 2
  gen.Offset = (-20*mm, -10*mm, -10*mm, 0*ns)
  gen.Sigma = (12*mm, 8*mm, 8*mm, 0*ns)
  kernel.generatorAction().adopt(gen)
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  """

  #VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of primary particles from LCIO input files
  """

  # First particle file reader
  gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/LCIO1");
  gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/e2e2nn_gen_1343_1.stdhep"
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/qq_gen_128_999.stdhep"
  gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/smuonLR_PointK_3TeV_BS_noBkg_run0001.stdhep"
  gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/bbbb_3TeV.stdhep"
  gen.OutputLevel = 4 # generator_output_level
  gen.Mask = 1
  gen.MomentumScale = 0.1
  kernel.generatorAction().adopt(gen)
  # Install vertex smearing for this interaction
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/Smear1");  
  gen.OutputLevel = 4 #generator_output_level
  gen.Mask = 1
  gen.Offset = (-20*mm, -10*mm, -10*mm, 0*ns)
  gen.Sigma = (12*mm, 8*mm, 8*mm, 0*ns)
  kernel.generatorAction().adopt(gen)

  # Second particle file reader
  gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/LCIO2");
  gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/e2e2nn_gen_1343_2.stdhep"
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/bbnn_3TeV_01.stdhep"
  gen.OutputLevel = 4 # generator_output_level
  gen.Mask = 2
  gen.MomentumScale = 0.1
  kernel.generatorAction().adopt(gen)
  # Install vertex smearing for this interaction
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/Smear2");
  gen.OutputLevel = generator_output_level
  gen.Mask = 2
  gen.Offset = (20*mm, 10*mm, 10*mm, 0*ns)
  gen.Sigma = (2*mm, 1*mm, 1*mm, 0*ns)
  kernel.generatorAction().adopt(gen)
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  # Merge all existing interaction records
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionMerger/InteractionMerger")
  gen.OutputLevel = 4 #generator_output_level
  kernel.generatorAction().adopt(gen)

  # Finally generate Geant4 primaries
  gen = DDG4.GeneratorAction(kernel,"Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = 4 #generator_output_level
  kernel.generatorAction().adopt(gen)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  #part.SaveProcesses = ['conv','Decay']
  part.SaveProcesses = ['Decay']
  part.OutputLevel = 4 # generator_output_level
  part.enableUI()
  #user = DDG4.Action(kernel,"Geant4UserParticleHandler/UserParticleHandler")
  #part.adopt(user)

  """
  rdr = DDG4.GeneratorAction(kernel,"LcioGeneratorAction/Reader")
  rdr.zSpread = 0.0
  rdr.lorentzAngle = 0.0
  rdr.OutputLevel = DDG4.OutputLevel.INFO
  rdr.Input = "LcioEventReader|test.data"
  rdr.enableUI()
  kernel.generatorAction().adopt(rdr)
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
  seq.adopt(f1)
  #seq.adopt(f4)
  act.adopt(f1)

  seq,act = simple.setupTracker('SiVertexEndcap')
  seq.adopt(f1)
  #seq.adopt(f4)

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
  phys.adopt(ph)

  phys.dump()

  kernel.configure()
  kernel.initialize()

  #DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
