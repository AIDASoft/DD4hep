
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
import os
import time
import DDG4
from DDG4 import OutputLevel as Output
from g4units import GeV, MeV, m, cm
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  args = DDG4.CommandLine()
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry(str("file:" + install_dir + "/examples/AlignDet/compact/AlephTPC.xml"))

  if args.alignments:
    kernel.loadXML(str("file:") + str(args.alignments))

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

  generator_output_level = Output.INFO

  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugMaterials = True
  act.DebugElements = False
  act.DebugVolumes = True
  act.DebugShapes = True
  seq, act = geant4.addDetectorConstruction("Geant4DetectorSensitivesConstruction/ConstructSD")

  # Setup particle gun
  pos = (0.0, 0.0, -364.0 * cm)
  gun = geant4.setupGun("Gun", particle='e+', energy=50 * GeV, multiplicity=1, position=pos)
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

  geant4.setupTracker('TPC')

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  phys.dump()
  geant4.execute()


if __name__ == "__main__":
  run()
