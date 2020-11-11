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
  m = Setup()
  if len(sys.argv) >= 2 and sys.argv[1] == "batch":
    DDG4.setPrintLevel(DDG4.OutputLevel.WARNING)
    m.kernel.UI = ''
  m.configure()
  m.defineOutput()
  m.setupGun()
  m.setupGenerator()
  m.setupPhysics()
  m.run()


if __name__ == "__main__":
  run()
