"""Base class for inputfile parameters"""

from DDSim.Helper.ConfigHelper import ConfigHelper

class Input( ConfigHelper ):
  """Configuration for the InputFiles"""
  def __init__( self ):
    super(Input, self).__init__()

    self.__parameters = {}

  def getParameters(self):
    return self.__parameters

  @property
  def _parameters( self ):
    return self.__parameters

  @_parameters.setter
  def _parameters( self, newParameters ):
    if isinstance( newParameters, dict ):
      for par, val in newParameters.iteritems():
        self.__parameters[par] = str(val)

    else:
      raise RuntimeError("Commandline setting of InputFileParameters is not supported, use a steeringFile: %s "
                         % newParameters )
