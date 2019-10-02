from __future__ import absolute_import, unicode_literals
import os, sys, DDG4
#
"""
   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  # Configure UI
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  geant4.setupCshUI(vis=True)
  if len(sys.argv) >= 2 and sys.argv[1] == "batch":
    kernel.UI = ''
  elif len(sys.argv) >= 3 and (sys.argv[1] == "batch" or sys.argv[2] == "batch"):
    kernel.UI = ''
  elif len(sys.argv) == 2 and sys.argv[1] != "batch":
    kernel.loadGeometry(sys.argv[1])
  elif len(sys.argv) == 3 and sys.argv[1] != "batch":
    kernel.loadGeometry(sys.argv[2])
  elif len(sys.argv) == 3 and sys.argv[2] != "batch":
    kernel.loadGeometry(sys.argv[1])
  #
  # Configure field
  geant4.setupTrackingField(prt=True)
  # Now build the physics list:
  phys = kernel.physicsList()
  phys.extends = 'QGSP_BERT'
  phys.enableUI()
  phys.dump()

  DDG4.setPrintLevel(DDG4.OutputLevel.DEBUG)
  geant4.execute()


if __name__ == "__main__":
  run()
