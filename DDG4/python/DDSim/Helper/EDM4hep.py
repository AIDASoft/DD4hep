"""Helper object for files containing one or more MCParticle collections"""

from DDSim.Helper.Input import Input
from DDSim.Helper.MCParticle import MCParticleMixin


class EDM4hep(Input, MCParticleMixin):
  """Configuration for the EDM4hep input files"""

  def __init__(self):
    super(EDM4hep, self).__init__()
    self._parameters["MCParticleCollectionName"] = "MCParticles"
    self._closeProperties()
