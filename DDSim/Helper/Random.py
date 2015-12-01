"""Helper object for random number generator objects"""

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
