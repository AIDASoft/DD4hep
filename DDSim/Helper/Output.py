"""Dummy helper object for particle gun properties"""

from DDSim.Helper.ConfigHelper import ConfigHelper
from DDSim.DD4hepSimulation import outputLevel

class Output( ConfigHelper ):
  """Output holding all gun properties so we can easily overwrite them via command line magic"""
  def __init__( self ):
    super(Output, self).__init__()
    self._kernel = outputLevel('INFO')
    self._part = outputLevel('INFO')
    self._inputStage = outputLevel('INFO')
    self._random = outputLevel('FATAL')

  @property
  def inputStage( self ):
    """Output level for input sources"""
    return self._inputStage
  @inputStage.setter
  def inputStage(self, level):
    self._inputStage = outputLevel(level)

  @property
  def kernel( self ):
    """Output level for Geant4 kernel"""
    return self._kernel
  @kernel.setter
  def kernel(self, level):
    self._kernel = outputLevel(level)

  @property
  def part( self ):
    """Output level for ParticleHandler"""
    return self._part
  @part.setter
  def part(self, level):
    self._part = outputLevel(level)

  @property
  def random( self ):
    """Output level for Random Number Generator setup"""
    return self._random
  @random.setter
  def random(self, level):
    self._random = outputLevel(level)
