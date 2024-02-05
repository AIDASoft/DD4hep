"""Helper object for SD Actions
"""

from DDSim.Helper.ConfigHelper import ConfigHelper


class Action(ConfigHelper):
  """Helper holding sensitive detector and other actions.

  The default tracker and calorimeter sensitive actions can be set with

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

  Additional actions can be set as well with the following syntax variations:

  >>> SIM = DD4hepSimulation()
  # single action by name only:
  >>> SIM.action.run = "Geant4TestRunAction"
  # multiple actions with comma-separated names:
  >>> SIM.action.event = "Geant4TestEventAction/Action0,Geant4TestEventAction/Action1"
  # single action by tuple of name and parameter dict:
  >>> SIM.action.track = ( "Geant4TestTrackAction", {"Property_int": 10} )
  # single action by dict of name and parameter dict:
  >>> SIM.action.step = { "name": "Geant4TestStepAction", "parameter": {"Property_int": 10} }
  # multiple actions by list of dict of name and parameter dict:
  >>> SIM.action.stack = [ { "name": "Geant4TestStackAction", "parameter": {"Property_int": 10} } ]

On the command line, these can be specified as JSON strings.

  """

  def __init__(self):
    super(Action, self).__init__()
    self._tracker = ('Geant4TrackerWeightedAction', {'HitPositionCombination': 2, 'CollectSingleDeposits': False})
    self._calo = 'Geant4ScintillatorCalorimeterAction'
    self._mapActions = dict()
    self._trackerSDTypes = ['tracker']
    self._calorimeterSDTypes = ['calorimeter']
    self._run = []
    self._event = []
    self._track = []
    self._step = []
    self._stack = []
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

  @staticmethod
  def makeListOfDictFromJSON(val):
    if isinstance(val, str):
      # assumes: valid JSON string or comma-separated list of names
      import json
      try:
        val = json.loads(val)
      except ValueError:
        val = [dict(name=v) for v in val.split(",")]
    if isinstance(val, tuple):
      # assumes: ( "Geant4TestEventAction", {"Property_int": 10} )
      # creates: { "name": "Geant4TestEventAction", "parameter": {"Property_int": 10} }
      # note: not able to specified as json which only allows a list
      val = dict(name=val[0], parameter=val[1])
    if isinstance(val, dict):
      # assumes: { "name": "Geant4TestEventAction", "parameter": {"Property_int": 10} }
      # creates: [ { "name": "Geant4TestEventAction", "parameter": {"Property_int": 10} } ]
      val = [val]
    if isinstance(val, list):
      # assumes: [ { "name": "Geant4TestEventAction", "parameter": {"Property_int": 10} } ]
      return val
    raise RuntimeError("Commandline setting of action is not successful for: %s " % val)

  @property
  def run(self):
    """ set the default run action """
    return self._run

  @run.setter
  def run(self, val):
    self._run.extend(Action.makeListOfDictFromJSON(val))

  @property
  def event(self):
    """ set the default event action """
    return self._event

  @event.setter
  def event(self, val):
    self._event.extend(Action.makeListOfDictFromJSON(val))

  @property
  def track(self):
    """ set the default track action """
    return self._track

  @track.setter
  def track(self, val):
    self._track.extend(Action.makeListOfDictFromJSON(val))

  @property
  def step(self):
    """ set the default step action """
    return self._step

  @step.setter
  def step(self, val):
    self._step.extend(Action.makeListOfDictFromJSON(val))

  @property
  def stack(self):
    """ set the default stack action """
    return self._stack

  @stack.setter
  def stack(self, val):
    self._stack.extend(Action.makeListOfDictFromJSON(val))
