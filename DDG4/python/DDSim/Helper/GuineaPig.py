"""Helper object for GuineaPig InputFile Parameters"""

from DDSim.Helper.Input import Input

class GuineaPig( Input ):
  """Configuration for the GuineaPig InputFiles"""
  def __init__( self ):
    super(GuineaPig, self).__init__()
    self._parameters["ParticlesPerEvent"] = -1

  @property
  def particlesPerEvent( self ):
    """Set the number of pair particles to simulate per event.
    Only used if inputFile ends with ".pairs"
    If "-1" all particles will be simulated in a single event
    """
    return self._parameters["ParticlesPerEvent"]
  @particlesPerEvent.setter
  def particlesPerEvent( self, val ):
    self._parameters["ParticlesPerEvent"] = str(val)
