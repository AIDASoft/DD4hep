"""Helper object for SD Actions"""

from DDSim.Helper.ConfigHelper import ConfigHelper


class Action( ConfigHelper ):
  """Action holding all gun properties"""
  def __init__( self ):
    super(Action, self).__init__()
    self._tracker = 'Geant4TrackerAction'
    self._calo = 'Geant4ScintillatorCalorimeterAction'
    self._mapActions = dict()

  @property
  def tracker( self ):
    """ set the default tracker action """
    return self._tracker
  @tracker.setter
  def tracker( self, val ):
    self._tracker = val

  @property
  def calo( self ):
    """ set the default calorimeter action """
    return self._calo
  @calo.setter
  def calo( self, val ):
    self._calo = val


  @property
  def mapActions( self ):
    """ create a map of patterns and actions to be applied to sensitive detectors
    e.g. tpc --> TPCSDAction """
    return self._mapActions

  @mapActions.setter
  def mapActions( self, val ):
    """check if the argument is a dict, then we just update mapActions
    if it is a string or list, we use pairs as patters --> Action
    """
    if isinstance(val, dict):
      self._mapActions.update(val)
      return

    if isinstance( val, basestring):
      vals = val.split(" ")
    elif isinstance( val, list ):
      vals = val
    if len(vals)%2 != 0:
      raise RuntimeError("Not enough parameters for mapActions")
    for index in xrange(0,len(vals),2):
      self._mapActions[vals[index]] = vals[index+1]

  def clearMapActions( self ):
    """empty the mapActions"""
    self._mapActions = dict()
