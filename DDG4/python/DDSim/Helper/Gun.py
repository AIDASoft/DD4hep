"""Helper object for particle gun properties"""

from __future__ import absolute_import, unicode_literals
from DDSim.Helper.ConfigHelper import ConfigHelper
from g4units import GeV
import logging
import ddsix as six

logger = logging.getLogger(__name__)


class Gun(ConfigHelper):
  """Configuration for the DDG4 ParticleGun"""

  def __init__(self):
    super(Gun, self).__init__()
    self.particle = "mu-"
    self.multiplicity = 1
    self._position = (0.0, 0.0, 0.0)
    self._isotrop = False
    self._direction = (0, 0, 1)

    self._phiMin_EXTRA = {'help': "Minimal azimuthal angle for random distribution"}
    self.phiMin = None
    self.phiMax = None
    self.thetaMin = None
    self.thetaMax = None
    self._momentumMin_EXTRA = {'help': "Minimal momentum when using distribution (default = 0.0)"}
    self.momentumMin = 0 * GeV
    self._momentumMax_EXTRA = {'help': "Maximal momentum when using distribution (default = 0.0)"}
    self.momentumMax = 10 * GeV
    self._energy_EXTRA = {'help': "The kinetic energy for the particle gun.\n\n"
                          "If not None, it will overwrite the setting of momentumMin and momentumMax"}
    self.energy = None

    self._distribution_EXTRA = {'choices': ['uniform', 'cos(theta)',
                                            'eta', 'pseudorapidity',
                                            'ffbar']}  # (1+cos^2 theta)
    self._distribution = None

  @property
  def distribution(self):
    """choose the distribution of the random direction for theta

    Options for random distributions:

    'uniform' is the default distribution, flat in theta
    'cos(theta)' is flat in cos(theta)
    'eta', or 'pseudorapidity' is flat in pseudorapity
    'ffbar' is distributed according to 1+cos^2(theta)

    Setting a distribution will set isotrop = True
    """
    return self._distribution

  @distribution.setter
  def distribution(self, val):
    if val is None:
      return
    possibleDistributions = self._distribution_EXTRA['choices']
    if not isinstance(val, six.string_types):
      raise RuntimeError("malformed input '%s' for gun.distribution. Need a string : %s " %
                         (val, ",".join(possibleDistributions)))
    if val not in possibleDistributions:
      # surround options by quots to be explicit
      stringified = ["'%s'" % _ for _ in possibleDistributions]
      raise RuntimeError("Unknown distribution '%s', Use one of: %s " % (val,
                                                                         ", ".join(stringified)))
    self._distribution = val
    self._isotrop = True

  @property
  def isotrop(self):
    """ isotropic distribution for the particle gun

    use the options phiMin, phiMax, thetaMin, and thetaMax to limit the range of randomly distributed directions
    if one of these options is not None the random distribution will be set to True and cannot be turned off!
    """
    return self._isotrop or bool(self._distribution)

  @isotrop.setter
  def isotrop(self, val):
    """check that value is equivalent to bool"""
    try:
      self._isotrop = ConfigHelper.makeBool(val)
    except RuntimeError:
      raise RuntimeError("malformed input '%s' for gun.isotrop " % val)
    if val and self.distribution is None:
      self.distribution = 'uniform'

  @property
  def direction(self):
    """ direction of the particle gun, 3 vector """
    return self._direction

  @direction.setter
  def direction(self, val):
    """ make sure the direction is parseable by boost, i.e. (1.0, 1.0, 1.0) """
    self._direction = ConfigHelper.makeTuple(val)
    if len(self._direction) != 3:
      raise RuntimeError(
          " gun.direction: malformed input '%s', needs to be a string representing a three vector " % (val,))

  @property
  def position(self):
    """ position of the particle gun, 3 vector """
    return self._position

  @position.setter
  def position(self, val):
    """check that the position is a three vector and can be parsed by ddg4"""
    self._position = ConfigHelper.makeTuple(val)
    if len(self._position) != 3:
      raise RuntimeError(
          " gun.position: malformed input '%s', needs to be a string representing a three vector " % (val,))

  def setOptions(self, ddg4Gun):
    """set the starting properties of the DDG4 particle gun"""
    try:
      if self.energy:
        ddg4Gun.Energy = self.energy
      ddg4Gun.particle = self.particle
      ddg4Gun.multiplicity = self.multiplicity
      ddg4Gun.position = self.position
      ddg4Gun.isotrop = self.isotrop
      ddg4Gun.direction = self.direction
      ddg4Gun.Distribution = self.distribution
      if self.thetaMin is not None:
        ddg4Gun.ThetaMin = self.thetaMin
        ddg4Gun.isotrop = True
      if self.thetaMax is not None:
        ddg4Gun.ThetaMax = self.thetaMax
        ddg4Gun.isotrop = True
      if self.phiMin is not None:
        ddg4Gun.PhiMin = self.phiMin
        ddg4Gun.isotrop = True
      if self.phiMax is not None:
        ddg4Gun.PhiMax = self.phiMax
        ddg4Gun.isotrop = True
      # this avoids issues if momentumMin is None because of previous default
      ddg4Gun.MomentumMin = self.momentumMin if self.momentumMin else 0.0
      ddg4Gun.MomentumMax = self.momentumMax
    except Exception as e:  # pylint: disable=W0703
      logger.error("parsing gun options:\n%s\nException: %s " % (self, e))
      exit(1)
