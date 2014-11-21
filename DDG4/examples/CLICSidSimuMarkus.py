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
  lcdd = kernel.lcdd()
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir+'/examples/DDG4/examples';
  kernel.loadGeometry("file:"+install_dir+"/DDDetectors/compact/SiD.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
  DDG4.importConstants(lcdd,debug=False)
  simple = DDG4.Simple(kernel,tracker='Geant4TrackerCombineAction')
  simple.printDetectors()
  # Configure UI
  simple.setupCshUI()

  # Configure Run actions
  run1 = DDG4.RunAction(kernel,'Geant4TestRunAction/RunInit')
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().adopt(run1)

  # Configure Event actions
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.DEBUG

  # Configure I/O
  ##evt_lcio = simple.setupLCIOOutput('LcioOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))
  ##evt_lcio.OutputLevel = generator_output_level
  ##evt_root = simple.setupROOTOutput('RootOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))

  gen = DDG4.GeneratorAction(kernel,"Geant4GeneratorActionInit/GenerationInit")
  gen.OutputLevel = generator_output_level
  kernel.generatorAction().adopt(gen)

  #VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of primary particles from LCIO input files
  """
  # First particle file reader
  gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/LCIO1");
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/e2e2nn_gen_1343_1.stdhep"
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/qq_gen_128_999.stdhep"
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/smuonLR_PointK_3TeV_BS_noBkg_run0001.stdhep"
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/bbbb_3TeV.stdhep"
  #gen.Input = "LCIOFileReader|/home/frankm/SW/data/mcparticles_pi-_5GeV.slcio"
  #gen.Input = "LCIOFileReader|/home/frankm/SW/data/mcparticles_mu-_5GeV.slcio"
  #gen.Input = "LCIOFileReader|/home/frankm/SW/data/bbbb_3TeV.slcio"
  #gen.Input = "LCIOStdHepReader|/home/frankm/SW/data/FCC-eh.stdhep"
  #gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/data.hepmc.txt"
  #gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/sherpa-2.1.1_zjets.hepmc2g"
  gen.Input = "LCIOFileReader|/afs/cern.ch/user/n/nikiforo/public/Markus/muons.slcio"
  #gen.Input = "LCIOFileReader|/afs/cern.ch/user/n/nikiforo/public/Markus/geantinos.slcio"

  gen.OutputLevel = generator_output_level
  gen.MomentumScale = 1.0
  gen.Mask = 1
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  # Merge all existing interaction records
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionMerger/InteractionMerger")
  gen.OutputLevel = generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # Finally generate Geant4 primaries
  gen = DDG4.GeneratorAction(kernel,"Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  #part.SaveProcesses = ['conv','Decay']
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100*MeV
  part.OutputLevel = Output.INFO #generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = DDG4.EcalEndcap_zmin
  user.TrackingVolume_Rmax = DDG4.EcalBarrel_rmin
  user.enableUI()
  part.adopt(user)

  """
  rdr = DDG4.GeneratorAction(kernel,"LcioGeneratorAction/Reader")
  rdr.zSpread = 0.0
  rdr.lorentzAngle = 0.0
  rdr.OutputLevel = DDG4.OutputLevel.INFO
  rdr.Input = "LcioEventReader|test.data"
  rdr.enableUI()
  kernel.generatorAction().adopt(rdr)
  """

  seq,act = simple.setupTracker('SiTrackerBarrel')
  """
  # First the tracking detectors
  seq,act = simple.setupTracker('SiVertexBarrel')
  seq,act = simple.setupTracker('SiVertexEndcap')
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
  """

  # Now build the physics list:
  phys = simple.setupPhysics('QGSP_BERT')
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
  kernel.terminate()

if __name__ == "__main__":
  run()
