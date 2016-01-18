"""Helper object for physicslist properties"""

from DDSim.Helper.ConfigHelper import ConfigHelper
from SystemOfUnits import mm

class Physics( ConfigHelper ):
  """Configuration for the PhysicsList"""
  def __init__( self ):
    super(Physics, self).__init__()
    self.rangecut = 0.7*mm
    self.list ="FTFP_BERT"
    self.decays = True

  def setupPhysics( self, kernel, name=None ):
    phys = kernel.physicsList()
    phys.extends  = name if name is not None else self.list
    phys.decays   = self.decays
    phys.rangecut = self.rangecut
    phys.enableUI()
    phys.dump()
    return phys
