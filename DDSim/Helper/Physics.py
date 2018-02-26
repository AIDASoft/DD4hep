"""Helper object for physicslist properties"""

import os

from DDSim.Helper.ConfigHelper import ConfigHelper
from SystemOfUnits import mm

class Physics( ConfigHelper ):
  """Configuration for the PhysicsList"""
  def __init__( self ):
    super(Physics, self).__init__()
    self._rangecut = 0.7*mm
    self.list ="FTFP_BERT"
    self.decays = True
    self._pdgfile = None


  @property
  def rangecut( self ):
    """ The global geant4 rangecut for secondary production

    Default is 0.7 mm as is the case in geant4 10

    To disable this plugin and be absolutely sure to use the Geant4 default range cut use "None"

    Set printlevel to DEBUG to see a printout of all range cuts,
    but this only works if range cut is not "None"
    """
    return self._rangecut
  @rangecut.setter
  def rangecut( self, val ):
    if val is None:
      self._rangecut = None
      return
    if isinstance( val, basestring):
      if val == "None":
        self._rangecut = None
        return
    self._rangecut = val

  @property
  def pdgfile( self ):
    """ location of particle.tbl file containing extra particles and their lifetime information

    For example in $DD4HEP/examples/DDG4/examples/particle.tbl
    """
    return self._pdgfile
  @pdgfile.setter
  def pdgfile( self, val ):
    if not val:
      self._pdgfile = None
      return
    if not os.path.exists( val ):
      raise RuntimeError( "PDGFile: %s not found" % os.path.abspath( val ) )
    self._pdgfile = os.path.abspath( val )


  def setupPhysics( self, kernel, name=None):
    seq = kernel.physicsList()
    seq.extends  = name if name is not None else self.list
    seq.decays   = self.decays
    seq.enableUI()
    seq.dump()

    from DDG4 import PhysicsList

    # Add special particle types from specialized physics constructor
    if self.pdgfile:
      seq = kernel.physicsList()
      part = PhysicsList(kernel, 'Geant4ExtraParticles/ExtraParticles')
      part.enableUI()
      seq.adopt(part)
      part.pdgfile = self.pdgfile

    # Add global range cut
    if self.rangecut is not None:
      seq = kernel.physicsList()
      rg = PhysicsList(kernel,'Geant4DefaultRangeCut/GlobalRangeCut')
      rg.enableUI()
      seq.adopt(rg)
      rg.RangeCut = self.rangecut

    return seq
