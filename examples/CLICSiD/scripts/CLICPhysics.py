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
   Subtest using CLICSid showing how to setup the Geant4 physics list

   @author  M.Frank
   @version 1.0

"""
if __name__ == "__main__":
  import CLICSid
  sid = CLICSid.CLICSid().loadGeometry()
  # <<-- See this function to know how it's done....
  sid.setupPhysics('QGSP_BERT')
  sid.test_run()
