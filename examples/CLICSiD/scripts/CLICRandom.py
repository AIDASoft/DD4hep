"""
   Subtest using CLICSid showing how to setup the random number generator

   @author  M.Frank
   @version 1.0

"""
from ROOT import TRandom
from ROOT import gRandom

if __name__ == "__main__":
  import logging, CLICSid, DDG4
  sid = CLICSid.CLICSid()
  
  logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
  logging.info('DEFAULT Engine: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  # <<-- See this function to know how it's done....
  rndm1 = sid.setupRandom('R1',seed=987654321,type='RunluxEngine')
  logging.info('R1: %s Default instance: %s', rndm1.name, rndm1.instance().name())
  logging.info('   Engine:  %s', rndm1.engine().name())
  logging.info('   DEFAULT: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  rndm1.showStatus()

  rndm2 = sid.setupRandom('R2',seed=1234321,type='MTwistEngine')
  logging.info('R2: %s Default instance: %s', rndm2.name, rndm2.instance().name())
  logging.info('   Engine:  %s', rndm2.engine().name())
  logging.info('   DEFAULT: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  #rndm2.showStatus()

  DDG4.Geant4Random.setMainInstance(rndm1.get())
  rndm1.showStatus()

  # Move main geant random instance from rndm1 to rndm2:
  # See how gRandom and HepRandom instances move
  DDG4.Geant4Random.setMainInstance(rndm1.get())
  logging.info('DEFAULT Engine: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  logging.info('DDG4   DEFAULT: %s', DDG4.Geant4Random.instance().engine().name())
  rndm = DDG4.Geant4Random.instance()

  rndm.setSeed(1234)
  rndm.showStatus()
  for i in xrange(10):
    logging.info("%s  -- 0 gRandome.Rndm()        -- Shoot random[[%d]= %f",rndm.name(),i,gRandom.Rndm())
  
  rndm.setSeed(1234)
  for i in xrange(10):
    logging.info("%s  -- 1 gRandome.Rndm()        -- Shoot random[[%d]= %f",rndm.name(),i,gRandom.Rndm())
  
  rndm.setSeed(1234)
  for i in xrange(10):
    logging.info("%s  -- 2 Geant4Random(CLHEP)    -- Shoot random[[%d]= %f",rndm.name(),i,rndm.rndm_clhep())
  
  rndm.setSeed(1234)
  for i in xrange(10):
    logging.info("%s  -- 3 Geant4Random(CLHEP)    -- Shoot random[[%d]= %f",rndm.name(),i,rndm.rndm_clhep())

  rndm.setSeed(1234)
  for i in xrange(10):
    logging.info("%s  -- 4 HepRandomEngine(CLHEP) -- Shoot random[%d]= %f",rndm.name(),i,rndm.engine().flat())
  
  sid.test_run(have_geo=False)
