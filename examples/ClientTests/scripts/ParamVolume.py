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
from __future__ import absolute_import, unicode_literals
import logging
#
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  import os
  import sys
  import time
  import DDG4
  args = DDG4.CommandLine()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  install_dir = install_dir + "/examples/ClientTests/compact"
  if args.help:
    logger.info("""
         python <dir>/ParamValue.py -option [-option]
              -geometry <geometry file name>  File is expected in the examples
                                              install area:
                                              """ + install_dir + """
              -vis                            Enable visualization
              -macro                          Pass G4 macro file to UI executive
              -batch                          Run in batch mode for unit testing
              -events <number>                Run geant4 for specified number of events
                                              (batch mode only)
    """)
    sys.exit(0)

  from DDG4 import OutputLevel as Output
  from g4units import GeV, MeV, mm

  kernel = DDG4.Kernel()
  kernel.loadGeometry(str("file:" + install_dir + os.sep + args.geometry))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, calo='Geant4CalorimeterAction')
  geant4.printDetectors()

  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro, vis=args.vis)
  else:
    ui = geant4.setupCshUI(vis=args.vis)
  if args.batch:
    ui.Commands = ['/run/beamOn ' + str(args.events), '/ddg4/UI/terminate']

  if args.sensitive:
    geant4.setupDetectors()

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.INFO

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugMaterials = True
  act.DebugElements = False
  act.DebugVolumes = True
  act.DebugShapes = True
  seq, act = geant4.addDetectorConstruction("Geant4DetectorSensitivesConstruction/ConstructSD")

  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'ParamVolume1D_' + time.strftime('%Y-%m-%d_%H-%M'))

  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='e+', energy=20 * GeV, multiplicity=1)
  gun.OutputLevel = generator_output_level

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, "Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100 * MeV
  part.OutputLevel = Output.INFO  # generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel, "Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 1 * mm
  user.TrackingVolume_Rmax = 1 * mm
  user.enableUI()
  part.adopt(user)

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, str('Geant4PhysicsList/Myphysics'))
  ph.addParticleConstructor(str('G4Geantino'))
  ph.addParticleConstructor(str('G4BosonConstructor'))
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  geant4.execute()


if __name__ == "__main__":
  run()
