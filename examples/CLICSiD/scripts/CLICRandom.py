"""
   Subtest using CLICSid showing how to setup the random number generator

   @author  M.Frank
   @version 1.0

"""
if __name__ == "__main__":
  import CLICSid
  sid = CLICSid.CLICSid()
  # <<-- See this function to know how it's done....
  rndm1=sid.setupRandom('R1',seed=987654321,type='MTwistEngine')
  rndm2=sid.setupRandom('R2',seed=1234321)

  # Move main geant random instance from rndm1 to rndm2:
  # See how gRandom and HepRandom instances move
  rndm2.setMainInstance(rndm2.get())
  rndm2.showStatus()
  rndm1.showStatus()
  sid.test_run(have_geo=False)
