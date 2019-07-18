"""
   Subtest using CLICSid showing how to setup the random number generator

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import
from ROOT import TRandom
from ROOT import gRandom

import logging

logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

if __name__ == "__main__":
  import CLICSid, DDG4
  sid = CLICSid.CLICSid()
  
  logger.info('DEFAULT Engine: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  # <<-- See this function to know how it's done....
  rndm1 = sid.setupRandom('R1',seed=987654321,type='RunluxEngine')
  logger.info('R1: %s Default instance: %s', rndm1.name, rndm1.instance().name())
  logger.info('   Engine:  %s', rndm1.engine().name())
  logger.info('   DEFAULT: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  rndm1.showStatus()

  rndm2 = sid.setupRandom('R2',seed=1234321,type='MTwistEngine')
  logger.info('R2: %s Default instance: %s', rndm2.name, rndm2.instance().name())
  logger.info('   Engine:  %s', rndm2.engine().name())
  logger.info('   DEFAULT: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  #rndm2.showStatus()

  DDG4.Geant4Random.setMainInstance(rndm1.get())
  rndm1.showStatus()

  # Move main geant random instance from rndm1 to rndm2:
  # See how gRandom and HepRandom instances move
  DDG4.Geant4Random.setMainInstance(rndm1.get())
  logger.info('DEFAULT Engine: %s', DDG4.CLHEP.HepRandom.getTheEngine().name())
  logger.info('DDG4   DEFAULT: %s', DDG4.Geant4Random.instance().engine().name())
  rndm = DDG4.Geant4Random.instance()

  rndm.setSeed(1234)
  rndm.showStatus()
  for i in xrange(10):
    logger.info("%s  -- 0 gRandome.Rndm()        -- Shoot random[[%d]= %f",rndm.name(),i,gRandom.Rndm())
  
  rndm.setSeed(1234)
  for i in xrange(10):
    logger.info("%s  -- 1 gRandome.Rndm()        -- Shoot random[[%d]= %f",rndm.name(),i,gRandom.Rndm())
  
  rndm.setSeed(1234)
  for i in xrange(10):
    logger.info("%s  -- 2 Geant4Random(CLHEP)    -- Shoot random[[%d]= %f",rndm.name(),i,rndm.rndm_clhep())
  
  rndm.setSeed(1234)
  for i in xrange(10):
    logger.info("%s  -- 3 Geant4Random(CLHEP)    -- Shoot random[[%d]= %f",rndm.name(),i,rndm.rndm_clhep())

  rndm.setSeed(1234)
  for i in xrange(10):
    logger.info("%s  -- 4 HepRandomEngine(CLHEP) -- Shoot random[%d]= %f",rndm.name(),i,rndm.engine().flat())
  
  sid.test_run(have_geo=False)
