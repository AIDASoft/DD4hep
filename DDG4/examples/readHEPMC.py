#
#
import os, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

DD4hep simulation example setup using the python configuration

@author  M.Frank
@version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  lcdd = kernel.lcdd()
  simple = DDG4.Simple(kernel)

  gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/Input")
  kernel.generatorAction().adopt(gen)
  gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/data.hepmc.txt";
  parts = gen.new_particles()
  gen.readParticles(0,parts);
  parts.clear()
  print 132*'*'
  print 132*'*'
  gen.readParticles(1,parts);

if __name__ == "__main__":
  run()
