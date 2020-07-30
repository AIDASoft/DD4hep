from __future__ import absolute_import, unicode_literals
import os
import sys
import DDG4
#
"""

   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  from MiniTelSetup import Setup
  m = Setup()
  if len(sys.argv) >= 2 and sys.argv[1] == "batch":
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    m.kernel.UI = ''
  m.configure()
  m.defineOutput()
  fname = os.environ['DD4hepExamplesINSTALL'] + '/examples/DDG4/data/hepmc_geant4.dat'
  m.setupInput("Geant4EventReaderHepMC|" + fname)
  m.setupGenerator()
  m.setupPhysics()
  m.run(num_events=1)


if __name__ == "__main__":
  run()
