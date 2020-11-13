"""

   DD4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0
   modified for LHeC

"""
from __future__ import absolute_import, unicode_literals
import logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def run():
  import LHeD
  import DDG4
  import os
  import g4units
  from DDG4 import OutputLevel as Output

  lhed = LHeD.LHeD()
  geant4 = lhed.geant4
  kernel = lhed.kernel
  lhed.loadGeometry()
  geant4.printDetectors()
  kernel.UI = "UI"
  geant4.setupCshUI()
  lhed.setupField(quiet=False)
  DDG4.importConstants(kernel.detectorDescription(), debug=False)

  dd4hep_dir = os.environ['DD4hep_DIR']
  kernel.loadXML("file:" + dd4hep_dir + "/examples/LHeD/scripts/DDG4_field.xml")

  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()

  # Configure G4 magnetic field tracking
  field = geant4.addConfig('Geant4FieldTrackingSetupAction/MagFieldTrackingSetup')
  field.stepper = "HelixGeant4Runge"
  field.equation = "Mag_UsualEqRhs"
  field.eps_min = 5e-0520 * g4units.mm
  field.eps_max = 0.001 * g4units.mm
  field.min_chord_step = 0.01 * g4units.mm
  field.delta_chord = 0.25 * g4units.mm
  field.delta_intersection = 1e-05 * g4units.mm
  field.delta_one_step = 0.001 * g4units.mm
  logger.info('+++++> %s -> stepper  = %s', field.name, field.stepper)
  logger.info('+++++> %s -> equation = %s', field.name, field.equation)
  logger.info('+++++> %s -> eps_min  = %s', field.name, field.eps_min)
  logger.info('+++++> %s -> eps_max  = %s', field.name, field.eps_max)
  logger.info('+++++> %s -> delta_one_step = %s', field.name, field.delta_one_step)

  """
  # Setup random generator
  rndm = DDG4.Action(kernel,'Geant4Random/Random')
  rndm.Seed = 987654321
  rndm.initialize()
  rndm.showStatus()
  rndm.Seed = 987654321
  """

  # Configure Run actions
  run1 = DDG4.RunAction(kernel, 'Geant4TestRunAction/RunInit')
  """
  run1.Property_int    = 12345
  run1.Property_double = -5e15*keV
  run1.Property_string = 'Startrun: Hello_LHeD'
  """
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().adopt(run1)

  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure I/O
  # evt_lcio = geant4.setupLCIOOutput('LcioOutput','Lhe_dip_sol_circ-higgs-bb')
  # evt_lcio.OutputLevel = Output.ERROR

  evt_root = geant4.setupROOTOutput('RootOutput', 'Lhe_dip_sol_circ-higgs-bb')
  evt_root.OutputLevel = Output.INFO

  gen = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)

  """
  # First particle generator: e-  non-isotropic generation using Gun:
  gun = DDG4.GeneratorAction(kernel,"Geant4ParticleGenerator/Gun");
  gun.Particle = 'e-'
  gun.Energy = 60 * GeV
  gun.Multiplicity = 1
  gun.Position = (0.*mm,0.*mm,0.*mm)
  gun.Direction = (1.,0.,0.)
  gun.Mask = 2
  gun.enableUI()
  kernel.generatorAction().adopt(gun)
  # Install vertex smearing for this primary e-
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearE-");
  gen.Mask = 1
  gen.Sigma = (0*mm, 0*mm, 0*mm, 0*ns)
  kernel.generatorAction().adopt(gen)
  """

  # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of isotrope tracks of a given multiplicity with overlay:
  """
  # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  # First particle generator: pi+
  gen = DDG4.GeneratorAction(kernel,"Geant4IsotropeGenerator/IsotropPi+");
  gen.Particle = 'pi+'
  gen.Energy = 200*GeV
  gen.Multiplicity = 1
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
  gen.Energy = 60 * GeV
  gen.Multiplicity = 2
  gen.Mask = 2
  kernel.generatorAction().adopt(gen)
  # Install vertex smearing for this interaction
  gen = DDG4.GeneratorAction(kernel,"Geant4InteractionVertexSmear/SmearE-");
  gen.Mask = 2
  gen.Offset = (-20*mm, -10*mm, -10*mm, 0*ns)
  gen.Sigma = (12*mm, 8*mm, 8*mm, 0*ns)
  kernel.generatorAction().adopt(gen)
  """
  # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of primary particles from LCIO input files
  """
  # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

  # First particle file reader
  gen = DDG4.GeneratorAction(kernel, "LCIOInputAction/LCIO1")
  # gen.Input = "LCIOStdHepReader|/afs/.cern.ch/project/lhec/"
  #             "software/aidasoft/DD4hep/DD4hep/files/NC_bb_tag_2_pythia_events.hep"
  # gen.Input = "LCIOStdHepReader|/opt/DD4hep/files/NC_bb_tag_2_pythia_events.hep"
  gen.Input = "LCIOStdHepReader|/opt/DD4hep/files/lhec_for_peter/tag_2_pythia_events.hep"
  # gen.Input = "LCIOStdHepReader|/opt/DD4hep/files/single-top-tag_1_pythia_events.hep"

  # gen.Input = "Geant4EventReaderHepMC|/opt/DD4hep/files/ePb-q2-0-i.mc2"
  # gen.Input = "LCIOStdHepReader|/opt/DD4hep/files/single-top-tag_1_pythia_events.hep"
  # gen.Input = "LCIOStdHepReader|/opt/DD4hep/files/root.hep"

  gen.OutputLevel = 2  # generator_output_level
  gen.MomentumScale = 1.0
  gen.Mask = 1
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # Install vertex smearing for this interaction
  gen = DDG4.GeneratorAction(kernel, "Geant4InteractionVertexSmear/Smear1")
  gen.OutputLevel = 4  # generator_output_level
  gen.Mask = 1
  gen.Offset = (-20 * g4units.mm, -10 * g4units.mm, -10 * g4units.mm, 0 * g4units.ns)
  gen.Sigma = (12 * g4units.mm, 8 * g4units.mm, 8 * g4units.mm, 0 * g4units.ns)
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  # Merge all existing interaction records
  gen = DDG4.GeneratorAction(kernel, "Geant4InteractionMerger/InteractionMerger")
  gen.OutputLevel = 4  # generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # Finally generate Geant4 primaries
  gen = DDG4.GeneratorAction(kernel, "Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = 4  # generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  # part.SaveProcesses = ['conv','Decay']
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 10 * g4units.MeV
  part.OutputLevel = 5  # generator_output_level
  part.enableUI()

  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = DDG4.EcalEndcap_zmin_fwd
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

  # Setup global filters fur use in sensntive detectors
  f1 = DDG4.Filter(kernel, 'GeantinoRejectFilter/GeantinoRejector')
  kernel.registerGlobalFilter(f1)
  f2 = DDG4.Filter(kernel, 'ParticleRejectFilter/OpticalPhotonRejector')
  f2.particle = 'opticalphoton'
  kernel.registerGlobalFilter(f2)
  f3 = DDG4.Filter(kernel, 'ParticleSelectFilter/OpticalPhotonSelector')
  f3.particle = 'opticalphoton'
  kernel.registerGlobalFilter(f3)

  f4 = DDG4.Filter(kernel, 'EnergyDepositMinimumCut')
  f4.Cut = 0.5 * g4units.MeV
  f4.enableUI()
  kernel.registerGlobalFilter(f4)

  # First the tracking detectors
  seq, act = geant4.setupTracker('SiVertexBarrel')
  seq.adopt(f1)
  act.adopt(f1)

  seq, act = geant4.setupTracker('SiTrackerBarrel')
  seq.adopt(f1)
  act.adopt(f1)
  seq, act = geant4.setupTracker('SiTrackerForward')
  seq.adopt(f1)
  act.adopt(f1)
  seq, act = geant4.setupTracker('SiTrackerBackward')
  seq.adopt(f1)
  act.adopt(f1)

  # Now the calorimeters
  seq, act = geant4.setupCalorimeter('EcalBarrel')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)

  seq, act = geant4.setupCalorimeter('EcalEndcap_fwd')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)
  seq, act = geant4.setupCalorimeter('EcalEndcap_bwd')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)

  seq, act = geant4.setupCalorimeter('HcalBarrel')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)
  seq, act = geant4.setupCalorimeter('HcalEndcap_fwd')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)
  seq, act = geant4.setupCalorimeter('HcalEndcap_bwd')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)

  seq, act = geant4.setupCalorimeter('HcalPlug_fwd')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)
  seq, act = geant4.setupCalorimeter('HcalPlug_bwd')
  seq.adopt(f3)
  act.adopt(f3)
  seq.adopt(f4)
  act.adopt(f4)

  seq, act = geant4.setupCalorimeter('MuonBarrel')
  seq.adopt(f2)
  act.adopt(f2)
  seq, act = geant4.setupCalorimeter('MuonEndcap_fwd1')
  seq.adopt(f2)
  act.adopt(f2)
  seq, act = geant4.setupCalorimeter('MuonEndcap_fwd2')
  seq.adopt(f2)
  act.adopt(f2)
  seq, act = geant4.setupCalorimeter('MuonEndcap_bwd1')
  seq.adopt(f2)
  act.adopt(f2)
  seq, act = geant4.setupCalorimeter('MuonEndcap_bwd2')
  seq.adopt(f2)
  act.adopt(f2)

  """
  scan = DDG4.SteppingAction(kernel,'Geant4MaterialScanner/MaterialScan')
  kernel.steppingAction().adopt(scan)
  """

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, 'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor(str('G4Geantino'))
  ph.addParticleConstructor(str('G4BosonConstructor'))
  ph.addParticleConstructor(str('G4LeptonConstructor'))
  ph.addParticleProcess(str('e[+-]'), str('G4eMultipleScattering'), -1, 1, 1)
  ph.addPhysicsConstructor(str('G4OpticalPhysics'))
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  kernel.configure()
  kernel.initialize()

  # DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  logging.info('End of run. Terminating .......')
  kernel.terminate()


if __name__ == "__main__":
  run()
