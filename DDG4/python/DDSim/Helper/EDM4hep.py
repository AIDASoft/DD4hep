"""Helper object for files containing one or more MCParticle collections"""

from DDSim.Helper.Input import Input
from DDSim.Helper.MCParticle import MCParticleMixin


class EDM4hep(Input, MCParticleMixin):
  """Configuration for the EDM4hep input files"""

  def __init__(self):
    super(EDM4hep, self).__init__()
    self._parameters["MCParticleCollectionName"] = "MCParticles"
    self._parameters["EventHeaderCollectionName"] = "EventHeader"
    self._closeProperties()

  @property
  def eventHeaderCollectionName(self):
    """Set the name of the collection containing the EventHeader input.
    """
    return self._parameters["EventHeaderCollectionName"]

  @eventHeaderCollectionName.setter
  def eventHeaderCollectionName(self, val):
    self._parameters["MCParticleCollectionName"] = val
