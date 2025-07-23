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
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    m.kernel.UI = ''
  m.configure()
  m.defineOutput()
  fname = os.environ['DD4hepExamplesINSTALL'] + '/examples/DDG4/data/Muons10GeV.HEPEvt'
  m.setupInput('Geant4EventReaderHepEvtShort|' + fname)
  m.setupGenerator()
  m.setupPhysics(model='FTFP_BERT')
  m.phys.decays = True
  m.run(num_events=1)


if __name__ == '__main__':
  run()
