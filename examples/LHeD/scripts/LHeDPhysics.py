"""
   Subtest using LHeD showing how to setup the Geant4 physics list

   @author  M.Frank
   @version 1.0

"""
if __name__ == "__main__":
  import LHeD
  lhed = LHeD.LHeD().loadGeometry()
  # <<-- See this function to know how it's done....
  lhed.setupPhysics('QGSP_BERT')
  lhed.test_run()
