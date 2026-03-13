"""Mixin class for mcParticleCollectionName for Input helpers."""


class MCParticleMixin():

  @property
  def mcParticleCollectionName(self):
    """Set the name of the collection containing the MCParticle input.
    """
    return self._parameters["MCParticleCollectionName"]

  @mcParticleCollectionName.setter
  def mcParticleCollectionName(self, val):
    self._parameters["MCParticleCollectionName"] = val
