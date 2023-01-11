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
  m = Setup(geometry='/examples/ClientTests/compact/MiniTelGenerate.xml')
  kernel = m.kernel
  if args.batch:
    run = '/run/beamOn ' + str(args.events)
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    m.ui.Commands = [run, '/ddg4/UI/terminate']
  m.configure()
  wr  = m.geant4.setupEDM4hepOutput('edm4hep', 'MiniTel_ddg4_edm4hep.root')
  gen = DDG4.GeneratorAction(kernel, 'Geant4GeneratorActionInit/GenerationInit')
  kernel.generatorAction().adopt(gen)

  gun = DDG4.GeneratorAction(kernel, 'Geant4IsotropeGenerator/IsotropPi+1')
  gun.Mask = 1 << 0
  gun.Particle = 'pi+'
  gun.Energy = 100 * GeV
  gun.Multiplicity = 1
  gun.Distribution = 'cos(theta)'
  kernel.generatorAction().adopt(gun)
  gun = None

  gen = DDG4.GeneratorAction(kernel, 'Geant4InteractionMerger/InteractionMerger')
  gen.enableUI()
  kernel.generatorAction().adopt(gen)

  gen = DDG4.GeneratorAction(kernel, 'Geant4PrimaryHandler/PrimaryHandler')
  gen.enableUI()
  kernel.generatorAction().adopt(gen)
  gen = None

  m.setupGenerator()
  m.setupPhysics()
  m.run()


if __name__ == '__main__':
  run()
