"""Helper object for random number generator objects"""

import random

from DDSim.Helper.ConfigHelper import ConfigHelper

class Random (ConfigHelper):
  """Properties for the random number generator"""
  def __init__ (self):
    super(Random, self).__init__()
    self.seed = None
    self.type = None
    self.luxury = 1
    self.replace_gRandom = True
    self.file = None
    self._random = None

    self._enableEventSeed_EXTRA = {'help': "If True, calculate random seed for each event based" \
                                           "on eventID and runID\nAllows reproducibility even when" \
                                           "SkippingEvents"}
    self.enableEventSeed = False

  def initialize(self, DDG4, kernel, output):
    """ initialize the random generator

    :param DDG4: DDG4 module
    :param kernel: Geant4 kernel
    :param int output: output level
    :returns: Geant4Random instance
    """
    if self._random:
      return self._random
    self._random = DDG4.Action(kernel,'Geant4Random/R1')

    if self.seed is None:
      ## System provided random source, truely random according to documentation
      self.seed = random.SystemRandom().randint(0, 2**31-1)
      print "Choosing random seed for you:", self.seed

    self._random.Seed = self.seed
    self._random.Luxury = self.luxury

    if self.type is not None:
      self._random.Type = self.type

    self._random.initialize()

    if self.seed is not None and self.enableEventSeed:
      self._eventseed = DDG4.RunAction(kernel,'Geant4EventSeed/EventSeeder1')

    ## Needs to be called after initilisation
    if output <= 3:
      self._random.showStatus()
    return self._random
