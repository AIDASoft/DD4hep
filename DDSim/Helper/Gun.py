"""Helper object for particle gun properties"""

from DDSim.Helper.ConfigHelper import ConfigHelper
from SystemOfUnits import GeV

class Gun( ConfigHelper ):
  """Configuration for the DDG4 ParticleGun"""
  def __init__( self ):
    super(Gun, self).__init__()
    self.energy = 10*GeV
    self.particle = "mu-"
    self.multiplicity = 1
    self._position = (0.0,0.0,0.0)
    self._isotrop = False
    self._direction = (0,0,1)
    self.phiMin = None
    self.phiMax = None
    self.thetaMin = None
    self.thetaMax = None

  @property
  def isotrop( self ):
    """ isotropic distribution for the particle gun """
    return self._isotrop
  @isotrop.setter
  def isotrop( self, val ):
    """check that value is equivalent to bool"""
    try:
      self._isotrop = ConfigHelper.makeBool( val )
    except RuntimeError:
      raise RuntimeError( "malformed input '%s' for gun.isotrop " % val)

  @property
  def direction( self ):
    """ direction of the particle gun, 3 vector """
    return self._direction
  @direction.setter
  def direction( self, val ):
    """ make sure the direction is parseable by boost, i.e. (1.0, 1.0, 1.0) """
    self._direction = ConfigHelper.makeTuple( val )
    if len(self._direction) != 3:
      raise RuntimeError(" gun.direction: malformed input '%s', needs to be a string representing a three vector " % val )


  @property
  def position( self ):
    """ position of the particle gun, 3 vector """
    return self._position
  @position.setter
  def position( self, val ):
    """check that the position is a three vector and can be parsed by ddg4"""
    self._position = ConfigHelper.makeTuple( val )
    if len(self._position) != 3:
      raise RuntimeError(" gun.position: malformed input '%s', needs to be a string representing a three vector " % val )
