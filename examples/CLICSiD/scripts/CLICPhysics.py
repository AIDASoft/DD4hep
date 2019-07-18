"""
   Subtest using CLICSid showing how to setup the Geant4 physics list

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import
if __name__ == "__main__":
  import CLICSid
  sid = CLICSid.CLICSid().loadGeometry()
  # <<-- See this function to know how it's done....
  sid.setupPhysics('QGSP_BERT')
  sid.test_run()
