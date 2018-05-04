"""
   Subtest using CLICSid showing how to setup the Geant4 physics list

   @author  M.Frank
   @version 1.0

"""
if __name__ == "__main__":
  from MiniTelSetup import Setup as MiniTel
  m = MiniTel()
  m.configure()
  logging.info("#  Configure G4 geometry setup")
  seq,act = m.geant4.addDetectorConstruction("Geant4DetectorGeometryConstruction/ConstructGeo")
  act.DebugRegions = True
  m.test_config(True)
  m.terminate()
