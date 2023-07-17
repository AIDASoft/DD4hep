"""Helper object for SD Actions
"""

from DDSim.Helper.ConfigHelper import ConfigHelper


class Action(ConfigHelper):
  """Helper holding sensitive detector actions.

  The default tracker and calorimeter actions can be set with

  >>> SIM = DD4hepSimulation()
  >>> SIM.action.tracker=('Geant4TrackerWeightedAction', {'HitPositionCombination': 2, 'CollectSingleDeposits': False})
  >>> SIM.action.calo = "Geant4CalorimeterAction"

  The default sensitive actions for calorimeters and trackers are applied based on the sensitive type.
  The list of sensitive types can be changed with

  >>> SIM = DD4hepSimulation()
  >>> SIM.action.trackerSDTypes = ['tracker', 'myTrackerSensType']
  >>> SIM.calor.calorimeterSDTypes = ['calorimeter', 'myCaloSensType']

  For specific subdetectors specific sensitive detectors can be set based on patterns in the name of the subdetector.

  >>> SIM = DD4hepSimulation()
  >>> SIM.action.mapActions['tpc'] = "TPCSDAction"

  and additional parameters for the sensitive detectors can be set when the map is given a tuple

  >>> SIM = DD4hepSimulation()
  >>> SIM.action.mapActions['ecal'] =( "CaloPreShowerSDAction", {"FirstLayerNumber": 1} )

  """

  def __init__(self):
    super(Action, self).__init__()
    self._tracker = ('Geant4TrackerWeightedAction', {'HitPositionCombination': 2, 'CollectSingleDeposits': False})
    self._calo = 'Geant4ScintillatorCalorimeterAction'
    self._mapActions = dict()
    self._trackerSDTypes = ['tracker']
    self._calorimeterSDTypes = ['calorimeter']
    self._closeProperties()

  @property
  def tracker(self):
    """ set the default tracker action """
    return self._tracker

  @tracker.setter
  def tracker(self, val):
    self._tracker = val

  @property
  def calo(self):
    """ set the default calorimeter action """
    return self._calo

  @calo.setter
  def calo(self, val):
    self._calo = val

  @property
  def mapActions(self):
    """Create a map of patterns and actions to be applied to sensitive detectors.

    Example: if the name of the detector matches 'tpc' the TPCSDAction is used.

      SIM.action.mapActions['tpc'] = "TPCSDAction"
    """
    return self._mapActions

  @mapActions.setter
  def mapActions(self, val):
    """check if the argument is a dict, then we just update mapActions
    if it is a string or list, we use pairs as patterns --> Action
    """
    if isinstance(val, dict):
      self._mapActions.update(val)
      return

    if isinstance(val, str):
      vals = val.split(" ")
    elif isinstance(val, list):
      vals = val
    if len(vals) % 2 != 0:
      raise RuntimeError("Not enough parameters for mapActions")
    for index in range(0, len(vals), 2):
      self._mapActions[vals[index]] = vals[index + 1]

  def clearMapActions(self):
    """empty the mapActions dictionary"""
    self._mapActions = dict()

  @property
  def trackerSDTypes(self):
    """List of patterns matching sensitive detectors of type Tracker."""
    return self._trackerSDTypes

  @trackerSDTypes.setter
  def trackerSDTypes(self, val):
    self._trackerSDTypes = ConfigHelper.makeList(val)

  @property
  def calorimeterSDTypes(self):
    """List of patterns matching sensitive detectors of type Calorimeter."""
    return self._calorimeterSDTypes

  @calorimeterSDTypes.setter
  def calorimeterSDTypes(self, val):
    self._calorimeterSDTypes = ConfigHelper.makeList(val)
