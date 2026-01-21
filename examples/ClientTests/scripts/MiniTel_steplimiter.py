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
import os
import DDG4
#
"""

   dd4hep example setup using the python configuration
   and reading a HepEvt event

   \author  M.Frank
   \version 1.0

"""


def run():
  from MiniTelSetup import Setup
  args = DDG4.CommandLine()
  m = Setup()
  if args.batch:
    m.kernel.UI = ''
  m.configure()
  m.defineOutput()
  fname = os.environ['DD4hepExamplesINSTALL'] + '/examples/DDG4/data/Muons10GeV.HEPEvt'
  m.setupInput('Geant4EventReaderHepEvtShort|' + fname)
  m.setupGenerator()
  m.setupPhysics(model='FTFP_BERT')
  #
  # Add customizable Geant4PhysicsList to Geant4PhysicsListActionSequence
  # with G4VModularPhysicsList FTP_BERT
  phys_list = m.geant4.addPhysics('MyPhysics')
  #
  # Add G4StepLimiterPhysics constructor to the Geant4PhysicsList.
  # Will be registered to the G4VModularPhysicsList later
  limiter_physics = phys_list.addPhysicsConstructorType('G4StepLimiterPhysics')
  limiter_physics.SetApplyToAll(True)
  #
  # Create output
  m.phys.dump()
  #
  # Run a a number of events
  if args.events:
    m.run(num_events=int(args.events))
  else:
    m.run(num_events=1)


if __name__ == '__main__':
  run()
