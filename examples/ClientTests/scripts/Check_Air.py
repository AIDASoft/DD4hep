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
"""
   dd4hep example setup using the python configuration

   \author  M.Frank
   \version 1.0

"""
from __future__ import absolute_import, unicode_literals
import logging
import sys


logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)


def help():
  logging.info("Check_shape.py -option [-option]                           ")
  logging.info("       -geometry   <geometry file>   Geometry file         ")
  logging.info("       -vis                          Enable visualization  ")
  logging.info("       -batch                        Batch execution       ")


def run():
  cnt = 0
  geo = None
  vis = False
  batch = False
  for i in sys.argv:
    cnt = cnt + 1
    c = i.upper()
    if c.find('BATCH') < 2 and c.find('BATCH') >= 0:
      batch = True
    elif c[:4] == '-GEO':
      geo = sys.argv[cnt]
    elif c[:4] == '-VIS':
      vis = True

  if not geo:
    help()
    sys.exit(1)

  import DDG4
  kernel = DDG4.Kernel()
  # Configure UI
  geant4 = DDG4.Geant4(kernel, tracker='Geant4TrackerCombineAction')
  if batch:
    ui = geant4.setupCshUI(typ=None, ui=None, vis=None)
    kernel.UI = 'UI'
  else:
    ui = geant4.setupCshUI(vis=vis)
  kernel.loadGeometry(geo)
  # Configure field
  geant4.setupTrackingField(prt=True)
  # Now build the physics list:
  geant4.setupPhysics('')
  kernel.physicsList().enableUI()
  DDG4.setPrintLevel(DDG4.OutputLevel.DEBUG)
  #
  ui.Commands = [
      '/ddg4/ConstructGeometry/printVolume /world_volume_1',
      '/ddg4/ConstructGeometry/printMaterial Air',
      '/ddg4/ConstructGeometry/printMaterial Vacuum',
      '/ddg4/UI/exit'
      ]
  kernel.NumEvents = 0
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()


# Main entry point:
if __name__ == "__main__":
  run()
