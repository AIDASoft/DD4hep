#
#
import os, time, getopt, DDG4
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

  gen = DDG4.GeneratorAction(kernel,"Geant4InputAction/Input")
  kernel.generatorAction().adopt(gen)
  gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/data.hepmc.txt"
  gen.Input = "Geant4EventReaderHepMC|/home/frankm/SW/data/Atlas_Pythia8.hepmc"
  gen.OutputLevel = Output.DEBUG
  parts = gen.new_particles()
  ret = 1
  while ret:
    ret = gen.readParticles(0,parts)
    parts.clear()
    print 132*'*',ret

if __name__ == "__main__":
  run()
