"""Helper object for particle gun properties"""

from DDSim.Helper.ConfigHelper import ConfigHelper
from SystemOfUnits import GeV

class Gun( ConfigHelper ):
  """Gun holding all gun properties"""
  def __init__( self ):
    super(Gun, self).__init__()
    self.energy = 10*GeV
    self.particle = "mu-"
    self.multiplicity = 1
    self.position = (0.0,0.0,0.0)
    self.isotrop = False
    self.direction = (0,0,1)


