"""

   Subtest using CLICSid showing the usage the HEPMC file reader

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import, unicode_literals

import logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def run():
  import CLICSid
  import DDG4
  import os
  from DDG4 import OutputLevel as Output

  sid = CLICSid.CLICSid(no_physics=False)
  geant4 = sid.geant4
  kernel = sid.kernel
  sid.loadGeometry()
  geant4.printDetectors()
  kernel.UI = 'UI'
  if len(sys.argv) >= 2 and sys.argv[1] == "batch":
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    kernel.UI = ''
  geant4.setupCshUI()
  sid.setupField(quiet=False)
  DDG4.importConstants(kernel.detectorDescription(), debug=False)

  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # First particle file reader
  gen = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)
  input = DDG4.GeneratorAction(kernel, "Geant4InputAction/Input")
  fname = os.environ['DD4hepExamplesINSTALL'] + '/examples/DDG4/data/hepmc_geant4.dat'
  input.Input = "Geant4EventReaderHepMC|" + fname
  input.MomentumScale = 1.0
  input.Mask = 1
  kernel.generatorAction().adopt(input)

  # Merge all existing interaction records
  merger = DDG4.GeneratorAction(kernel, "Geant4InteractionMerger/InteractionMerger")
  merger.enableUI()
  kernel.generatorAction().adopt(merger)

  logger.info("#  Finally generate Geant4 primaries")
  gen = DDG4.GeneratorAction(kernel, "Geant4PrimaryHandler/PrimaryHandler")
  gen.OutputLevel = 4  # generator_output_level
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.OutputLevel = Output.INFO
  part.enableUI()

  logger.info("#  Configure Event actions")
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = DDG4.EcalEndcap_zmin
  user.TrackingVolume_Rmax = DDG4.EcalBarrel_rmin
  user.enableUI()
  part.adopt(user)
  #
  sid.setupDetectors()
  sid.setupPhysics('QGSP_BERT')
  sid.test_run(have_geo=True, num_events=1)


if __name__ == "__main__":
  run()
