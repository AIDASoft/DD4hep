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
"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import, unicode_literals
import os
import DDG4


def run():
  kernel = DDG4.Kernel()
  install_dir = os.environ['DD4hepExamplesINSTALL']
  kernel.loadGeometry("file:" + install_dir + "/examples/OpticalSurfaces/compact/OpNovice.xml")


if __name__ == "__main__":
  run()
