"""Dummy helper object for particle gun properties"""

from DDSim.Helper.ConfigHelper import ConfigHelper

OUTPUT_CHOICES = (1, 2, 3, 4, 5, 6, 7, 'VERBOSE', 'DEBUG',
                  'INFO', 'WARNING', 'ERROR', 'FATAL', 'ALWAYS')


def outputLevelType(level):
  """Return verbosity level as integer if possible.

  Still benefit from argparsers list of possible choices
  """
  try:
    return int(level)
  except ValueError:
    return str(level)


def outputLevel(level):
  """return INT for outputlevel"""
  if isinstance(level, int):
    if level < 1 or 7 < level:
      raise KeyError
    return level
  outputlevels = {"VERBOSE": 1,
                  "DEBUG": 2,
                  "INFO": 3,
                  "WARNING": 4,
                  "ERROR": 5,
                  "FATAL": 6,
                  "ALWAYS": 7}
  return outputlevels[level.upper()]


class Output(ConfigHelper):
  """Configuration for the output levels of DDG4 components"""

  def __init__(self):
    super(Output, self).__init__()
    self._kernel_EXTRA = {'choices': OUTPUT_CHOICES, 'type': outputLevelType}
    self._kernel = outputLevel('INFO')

    self._part_EXTRA = {'choices': OUTPUT_CHOICES, 'type': outputLevelType}
    self._part = outputLevel('INFO')

    self._inputStage_EXTRA = {'choices': OUTPUT_CHOICES, 'type': outputLevelType}
    self._inputStage = outputLevel('INFO')

    self._random_EXTRA = {'choices': OUTPUT_CHOICES, 'type': outputLevelType}
    self._random = outputLevel('FATAL')

    self._geometry_EXTRA = {'choices': OUTPUT_CHOICES, 'type': outputLevelType}
    self._geometry = outputLevel('DEBUG')

  @property
  def inputStage(self):
    """Output level for input sources"""
    return self._inputStage

  @inputStage.setter
  def inputStage(self, level):
    self._inputStage = outputLevel(level)

  @property
  def kernel(self):
    """Output level for Geant4 kernel"""
    return self._kernel

  @kernel.setter
  def kernel(self, level):
    self._kernel = outputLevel(level)

  @property
  def part(self):
    """Output level for ParticleHandler"""
    return self._part

  @part.setter
  def part(self, level):
    self._part = outputLevel(level)

  @property
  def random(self):
    """Output level for Random Number Generator setup"""
    return self._random

  @random.setter
  def random(self, level):
    self._random = outputLevel(level)

  @property
  def geometry(self):
    """Output level for geometry."""
    return self._geometry

  @geometry.setter
  def geometry(self, level):
    self._geometry = outputLevel(level)
