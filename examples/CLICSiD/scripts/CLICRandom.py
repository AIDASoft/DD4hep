"""
   Subtest using CLICSid showing how to setup the random number generator

   @author  M.Frank
   @version 1.0

"""
from ROOT import TRandom
from ROOT import gRandom

if __name__ == "__main__":
  import CLICSid, DDG4
  sid = CLICSid.CLICSid()
  print 'DEFAULT Engine:', DDG4.CLHEP.HepRandom.getTheEngine().name()
  # <<-- See this function to know how it's done....
  rndm1 = sid.setupRandom('R1',seed=987654321,type='RunluxEngine')
  print 'R1:', rndm1.name, ' Default instance:', rndm1.instance().name()
  print '   Engine: ', rndm1.engine().name()
  print '   DEFAULT:', DDG4.CLHEP.HepRandom.getTheEngine().name()
  rndm1.showStatus()

  rndm2 = sid.setupRandom('R2',seed=1234321,type='MTwistEngine')
  print 'R2:', rndm2.name, ' Default instance:', rndm2.instance().name()
  print '   Engine:', rndm2.engine().name()
  print '   DEFAULT:', DDG4.CLHEP.HepRandom.getTheEngine().name()
  #rndm2.showStatus()

  DDG4.Geant4Random.setMainInstance(rndm1.get())
  rndm1.showStatus()

  # Move main geant random instance from rndm1 to rndm2:
  # See how gRandom and HepRandom instances move
  DDG4.Geant4Random.setMainInstance(rndm1.get())
  print 'DEFAULT Engine:', DDG4.CLHEP.HepRandom.getTheEngine().name()
  print 'DDG4   DEFAULT:', DDG4.Geant4Random.instance().engine().name()
  rndm = DDG4.Geant4Random.instance()

  rndm.setSeed(1234)
  rndm.showStatus()
  for i in xrange(10):
    print rndm.name(), ' -- 0 gRandome.Rndm()        -- Shoot random[',i,']= ',gRandom.Rndm()
  
  rndm.setSeed(1234)
  for i in xrange(10):
    print rndm.name(), ' -- 1 gRandome.Rndm()        -- Shoot random[',i,']= ',gRandom.Rndm()
  
  rndm.setSeed(1234)
  for i in xrange(10):
    print rndm.name(), ' -- 2 Geant4Random(CLHEP)    -- Shoot random[',i,']= ',rndm.rndm_clhep()
  
  rndm.setSeed(1234)
  for i in xrange(10):
    print rndm.name(), ' -- 3 Geant4Random(CLHEP)    -- Shoot random[',i,']= ',rndm.rndm_clhep()

  rndm.setSeed(1234)
  for i in xrange(10):
    print rndm.name(), ' -- 4 HepRandomEngine(CLHEP) -- Shoot random[',i,']= ',rndm.engine().flat()
  
  sid.test_run(have_geo=False)
