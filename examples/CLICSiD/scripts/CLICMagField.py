"""
   Subtest using CLICSid showing how to manipulate the magnetic field

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import
if __name__ == "__main__":
  import CLICSid
  sid = CLICSid.CLICSid().loadGeometry()
  # <<-- See this function to know how it's done....
  sid.setupField(quiet=False)
  sid.test_run()
