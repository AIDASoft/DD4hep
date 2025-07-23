# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
#
#
import logging
#
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)
#
#
"""

   dd4hep simulation example setup using the python configuration

   NOTE:
   If you get to the command prompt, you must not forget to enable GFlash!
   By default Geant4 does not enable it. Hence:
   Idle>  /GFlash/flag 1

   @author  M.Frank
   @version 1.0

"""


def run():
  import os
  import time
  import DDG4
  args = DDG4.CommandLine()
  if args.help:
    import sys
    logger.info("""
          python <dir>/ParamValue.py -option [-option]
              -geometry <geometry file>       Geometry with full path
              -vis                            Enable visualization
              -macro                          Pass G4 macro file to UI executive
              -batch                          Run in batch mode for unit testing
              -events <number>                Run geant4 for specified number of events
                                              (batch mode only)
    """)
    sys.exit(0)

  from DDG4 import OutputLevel as Output
  from g4units import GeV, MeV, m
  kernel = DDG4.Kernel()
  if args.geometry:
    kernel.loadGeometry(str("file:" + args.geometry))
  else:
    install_dir = os.environ['DD4hepExamplesINSTALL']
    kernel.loadGeometry(str("file:" + install_dir + "/examples/ClientTests/compact/SiliconBlock.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction', calo='Geant4CalorimeterAction')
  geant4.printDetectors()
  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro, vis=args.vis)
  else:
    ui = geant4.setupCshUI()
  if args.batch:
    ui.Commands = ['/run/beamOn ' + str(args.events), '/ddg4/UI/terminate']

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  kernel.eventAction().adopt(prt)

  generator_output_level = prt.OutputLevel

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction('Geant4DetectorGeometryConstruction/ConstructGeo')
  act.DebugMaterials = True
  act.DebugElements = False
  act.DebugVolumes = True
  act.DebugShapes = True

  # Apply sensitive detectors
  sensitives = DDG4.DetectorConstruction(kernel, str('Geant4DetectorSensitivesConstruction/ConstructSD'))
  sensitives.enableUI()
  seq.adopt(sensitives)

  # Enable GFlash shower model
  model = DDG4.DetectorConstruction(kernel, str('Geant4GFlashShowerModel/ShowerModel'))
  model.Parametrization = 'GFlashHomoShowerParameterisation'
  # Mandatory model parameters
  model.RegionName = 'SiRegion'
  model.Material = 'Silicon'
  model.Enable = True
  # Energy boundaries are optional: Units are GeV
  model.Emin = {'e+': 0.1 * GeV, 'e-': 0.1 * GeV}
  model.Emax = {'e+': 100 * GeV, 'e-': 100 * GeV}
  model.Ekill = {'e+': 0.1 * MeV, 'e-': 0.1 * MeV}
  model.enableUI()
  seq.adopt(model)

  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'SiliconBlock_GFlash_' + time.strftime('%Y-%m-%d_%H-%M'))

  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='e+', energy=50 * GeV, multiplicity=1)
  gun.OutputLevel = generator_output_level

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100 * MeV
  part.OutputLevel = Output.INFO  # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 3.0 * m
  user.TrackingVolume_Rmax = 3.0 * m
  user.enableUI()
  part.adopt(user)

  geant4.setupTracker('SiliconBlockUpper')
  geant4.setupTracker('SiliconBlockDown')

  # Now build the physics list:
  phys = geant4.setupPhysics('FTFP_BERT')
  ph = DDG4.PhysicsList(kernel, str('Geant4FastPhysics/FastPhysicsList'))
  ph.EnabledParticles = ['e+', 'e-']
  ph.BeVerbose = True
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  geant4.execute()


if __name__ == "__main__":
  run()
