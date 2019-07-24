"""
   Subtest using LHeD showing how to manipulate the magnetic field

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import, unicode_literals
if __name__ == "__main__":
  import LHeD
  lhed = LHeD.LHeD().loadGeometry()
  # <<-- See this function to know how it's done....
  lhed.setupField(quiet=False)
  lhed.test_run()
