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
  from MiniTelSetup import Setup
  args = DDG4.CommandLine()

  m = Setup(geometry=args.geometry)
  cmds = []
  if args.events:
    cmds = ['/run/beamOn ', str(args.events)]

  if args.batch:
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    cmds.append('/ddg4/UI/terminate')
    m.kernel.UI = ''

  if args.debug:
    # Configure G4 geometry setup
    seq, act = m.geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
    act.DebugVolumes = True
    act.DebugRegions = True
    act.DebugLimits  = True
    seq, act = m.geant4.addDetectorConstruction("Geant4DetectorSensitivesConstruction/ConstructSD")

  m.ui.Commands = cmds
  m.configure()
  m.defineOutput()
  m.setupGun()
  m.setupGenerator()
  m.setupPhysics()
  m.run()


if __name__ == "__main__":
  run()
