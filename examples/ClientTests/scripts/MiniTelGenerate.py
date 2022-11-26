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
from __future__ import absolute_import, unicode_literals
import sys
import DDG4
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  from g4units import GeV
  from MiniTelSetup import Setup
  args = DDG4.CommandLine()
  m = Setup(geometry="/examples/ClientTests/compact/MiniTelGenerate.xml")
  kernel = m.kernel
  if args.batch:
    run = '/run/beamOn ' + str(args.events)
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    cmds = []
    if not args.runs:
      args.runs = 1
    for i in range(int(args.runs)):
      cmds.append(run)
    cmds.append('/ddg4/UI/terminate')
    m.ui.Commands = cmds
  m.configure()
  wr = m.defineOutput(output='MiniTel')
  wr.FilesByRun = True
  gen = DDG4.GeneratorAction(kernel, "Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)

  gun = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropPi+")
  gun.Mask = 1
  gun.Particle = 'pi+'
  gun.Energy = 100 * GeV
  gun.Multiplicity = 1
  gun.Distribution = 'cos(theta)'
  kernel.generatorAction().adopt(gun)

  gun = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropPi-")
  gun.Mask = 2
  gun.Particle = 'pi-'
  gun.Energy = 100 * GeV
  gun.Multiplicity = 1
  gun.Distribution = 'cos(theta)'
  kernel.generatorAction().adopt(gun)

  gun = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropE+1")
  gun.Mask = 4
  gun.Particle = 'e+'
  gun.Energy = 50 * GeV
  gun.Multiplicity = 1
  gun.Distribution = 'cos(theta)'
  kernel.generatorAction().adopt(gun)

  gun = DDG4.GeneratorAction(kernel, "Geant4IsotropeGenerator/IsotropE+2")
  gun.Mask = 8
  gun.Particle = 'e+'
  gun.Energy = 100 * GeV
  gun.Multiplicity = 1
  gun.Distribution = 'cos(theta)'
  kernel.generatorAction().adopt(gun)
  gun = None

  gen = DDG4.GeneratorAction(kernel, "Geant4InteractionMerger/InteractionMerger")
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  gen = DDG4.GeneratorAction(kernel, "Geant4PrimaryHandler/PrimaryHandler")
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  gen = None

  m.setupGenerator()
  m.setupPhysics()
  m.run()


if __name__ == "__main__":
  run()
