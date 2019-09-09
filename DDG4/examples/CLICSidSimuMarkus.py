#
#
from __future__ import absolute_import, unicode_literals
import os
import time
import logging
import DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

import os, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *

kernel = DDG4.Kernel()
description = kernel.detectorDescription()
install_dir = os.environ['DD4hepINSTALL']
example_dir = install_dir+'/examples/DDG4/examples';
kernel.loadGeometry("file:"+install_dir+"/DDDetectors/compact/SiD_Markus.xml")
kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
DDG4.importConstants(description,debug=False)
geant4 = DDG4.Geant4(kernel,tracker='Geant4TrackerCombineAction')
geant4.printDetectors()
# Configure UI
geant4.setupCshUI()

a = DDG4.PhaseAction(kernel,'Geant4FieldPhaseAction/Geant4FieldPhaseAction_1')
kernel.phase('configure').add(a)

kernel.configure()


"""


def run():
  kernel = DDG4.Kernel()
  description = kernel.detectorDescription()
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir + '/examples/DDG4/examples'
  kernel.loadGeometry(str("file:" + install_dir + "/DDDetectors/compact/SiD_Markus.xml"))
  # kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
  DDG4.importConstants(description, debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  # geant4.setupCshUI(macro='run.mac',ui=None)
  geant4.setupCshUI()
  geant4.setupTrackingField()

  # Configure Run actions
  run1 = DDG4.RunAction(kernel, 'Geant4TestRunAction/RunInit')
  run1.enableUI()
  kernel.registerGlobalAction(run1)
  kernel.runAction().adopt(run1)

  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.WARNING
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.WARNING

  # Configure I/O
  evt_lcio = geant4.setupLCIOOutput('LcioOutput', 'CLICSiD_' + time.strftime('%Y-%m-%d_%H-%M'))
  ##evt_lcio.OutputLevel = generator_output_level
  #evt_root = geant4.setupROOTOutput('RootOutput','CLICSiD_'+time.strftime('%Y-%m-%d_%H-%M'))

  prim = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  # VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
  """
  Generation of primary particles from LCIO input files
  """
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
  gen.MomentumScale = 1.0
  gen.Mask = 1
  geant4.buildInputStage([gen],output_level=generator_output_level)
  """
  #^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  gen = geant4.setupGun("Gun", particle='mu+', energy=20 * GeV, position=(0 * mm, 0 * mm, 0 * cm), multiplicity=3)
  gen.isotrop = True
  gen.direction = (1, 0, 0)
  gen.OutputLevel = generator_output_level
  gen.Standalone = False
  """
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
  geant4.buildInputStage([prim, gen], Output.ERROR)

  """
  """

  """
  rdr = DDG4.GeneratorAction(kernel,"LcioGeneratorAction/Reader")
  rdr.zSpread = 0.0
  rdr.lorentzAngle = 0.0
  rdr.OutputLevel = DDG4.OutputLevel.INFO
  rdr.Input = "LcioEventReader|test.data"
  rdr.enableUI()
  kernel.generatorAction().adopt(rdr)
  """

  # First the tracking detectors
  seq, act = geant4.setupTracker('SiTrackerBarrel')
  seq, act = geant4.setupTracker('SiTrackerEndcap')
  seq, act = geant4.setupTracker('SiTrackerForward')
  """
  # Now the calorimeters
  seq,act = geant4.setupTracker('SiVertexBarrel')
  seq,act = geant4.setupTracker('SiVertexEndcap')

  seq,act = geant4.setupCalorimeter('EcalBarrel')
  seq,act = geant4.setupCalorimeter('EcalEndcap')
  seq,act = geant4.setupCalorimeter('HcalBarrel')
  seq,act = geant4.setupCalorimeter('HcalEndcap')
  seq,act = geant4.setupCalorimeter('HcalPlug')
  seq,act = geant4.setupCalorimeter('MuonBarrel')
  seq,act = geant4.setupCalorimeter('MuonEndcap')
  seq,act = geant4.setupCalorimeter('LumiCal')
  seq,act = geant4.setupCalorimeter('BeamCal')
  """
  """
  scan = DDG4.SteppingAction(kernel,'Geant4MaterialScanner/MaterialScan')
  kernel.steppingAction().adopt(scan)
  """

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, 'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4Geantino')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  kernel.configure()
  kernel.initialize()

  # DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  logger.info('End of run. Terminating .......')
  kernel.terminate()


if __name__ == "__main__":
  run()
