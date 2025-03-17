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
from g4units import GeV, MeV, m
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
  logger = DDG4.Logger('BoxOfStraws')
  install_dir = os.environ['DD4hepExamplesINSTALL']

  if args.sensitive:
    kernel.loadGeometry(str('file:' + install_dir + '/examples/ClientTests/compact/BoxOfStraws_sensitive.xml'))
  else:
    kernel.loadGeometry(str('file:' + install_dir + '/examples/ClientTests/compact/BoxOfStraws.xml'))

  DDG4.importConstants(kernel.detectorDescription(), debug=False)
  geant4 = DDG4.Geant4(kernel)
  geant4.printDetectors()
  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro)
  else:
    ui = geant4.setupCshUI()

  if args.verbose:
    ui.Commands.append('/run/verbose ' + str(args.verbose))
  if args.batch:
    ui.Commands.append('/run/beamOn ' + str(args.events))

  # Terminate sequence
  if args.batch:
    ui.Commands.append('/ddg4/UI/terminate')
  #
  # Configure field
  geant4.setupTrackingField(prt=True)
  #
  prt = DDG4.EventAction(kernel, 'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType = 3  # Print both: table and tree
  kernel.eventAction().adopt(prt)
  #
  seq, act = geant4.addDetectorConstruction('Geant4DetectorConstructionResources/ResourcesBeforeConstruction')
  act.When = "geometry"
  #
  # Configure G4 geometry setup
  seq, act = geant4.addDetectorConstruction('Geant4DetectorGeometryConstruction/ConstructGeo')
  act.DebugVolumes = True
  #
  if args.sensitive:
    # Assign sensitive detectors according to the declarations 'tracker' or 'calorimeter', etc
    seq, act = geant4.addDetectorConstruction('Geant4DetectorSensitivesConstruction/ConstructSD')
  else:
    # Assign sensitive detectors in Geant4 by matching a regular expression in the detector sub-tree
    seq, act = geant4.addDetectorConstruction('Geant4RegexSensitivesConstruction/ConstructSDRegEx')
    act.Detector = 'BoxOfStrawsDet'
    act.OutputLevel = Output.ALWAYS
    act.Match = ['gas_']
  #
  seq, act = geant4.addDetectorConstruction('Geant4DetectorConstructionResources/ResourcesAfterConstruction')
  act.When = "sensitives"
  #
  # Configure I/O
  geant4.setupROOTOutput('RootOutput', 'BoxOfStraws_' + time.strftime('%Y-%m-%d_%H-%M'))
  #
  # Setup particle gun
  gun = geant4.setupGun('Gun', particle='pi+', energy=10 * GeV, multiplicity=1)
  gun.OutputLevel = Output.INFO
  gun.enableUI()
  #
  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel, 'Geant4ParticleHandler/ParticleHandler')
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 50 * MeV
  user = DDG4.Action(kernel, 'Geant4TCUserParticleHandler/UserParticleHandler')
  user.TrackingVolume_Zmax = 2.5 * m
  user.TrackingVolume_Rmax = 2.5 * m
  part.adopt(user)
  #
  # Map sensitive detectors of type 'BoxOfStraws' to Geant4CalorimeterAction
  sd = geant4.description.sensitiveDetector(str('BoxOfStrawsDet'))
  logger.info(f'+++ BoxOfStraws: SD type: {str(sd.type())}')
  energy_filter = DDG4.Filter(kernel, 'EnergyDepositMinimumCut')
  energy_filter.Cut = 1.0 * MeV
  energy_filter.enableUI()
  kernel.registerGlobalFilter(energy_filter)
  seq, act = geant4.setupDetector(name='BoxOfStrawsDet', action='MyTrackerSDAction')
  seq.adopt(energy_filter)
  act.HaveCellID = False
  #
  # Now build the physics list:
  phys = geant4.setupPhysics(str('QGSP_BERT'))
  phys.dump()
  if args.simultate:
    geant4.execute()
  else:
    geant4.kernel().configure()
    geant4.kernel().initialize()
    geant4.kernel().terminate()


if __name__ == "__main__":
  run()
