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
from __future__ import absolute_import, unicode_literals
import os
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV

"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  args = DDG4.CommandLine()
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/DDCAD/compact/DD4hep_Issue_1134.xml"))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.printDetectors()
  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro)
  else:
    ui = geant4.setupCshUI()
  if args.batch:
    ui.Commands = ['/run/beamOn ' + str(args.events), '/ddg4/UI/terminate']

  # Configure field
  geant4.setupTrackingField(prt=True)
  # Configure Event actions
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugVolumes = True
  act.DebugShapes = True
  seq, act = geant4.addDetectorConstruction("Geant4DetectorSensitivesConstruction/ConstructSD")

  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'DD4hep_Issue_1134_' + time.strftime('%Y-%m-%d_%H-%M'))
  # Setup particle gun
  gun = geant4.setupGun("Gun", particle='e+', energy=20 * GeV, multiplicity=1)
  gun.OutputLevel = Output.INFO
  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel, str('Geant4PhysicsList/Myphysics'))
  ph.addParticleConstructor(str('G4Geantino'))
  ph.enableUI()
  phys.adopt(ph)
  geant4.execute()


if __name__ == "__main__":
  run()
