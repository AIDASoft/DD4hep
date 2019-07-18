"""Helper object for physicslist properties"""

from __future__ import absolute_import
import os

from DDSim.Helper.ConfigHelper import ConfigHelper
from g4units import mm

class Physics( ConfigHelper ):
  """Configuration for the PhysicsList"""
  def __init__( self ):
    super(Physics, self).__init__()
    self._rangecut = 0.7*mm
    self._list = "FTFP_BERT"
    self._decays = False
    self._pdgfile = None
    self._rejectPDGs = {1,2,3,4,5,6,21,23,24,25}
    self._zeroTimePDGs = {11, 13, 15, 17}

  @property
  def rejectPDGs( self ):
    """Set of PDG IDs that will not be passed from the input record to Geant4.

    Quarks, gluons and W's Z's etc should not be treated by Geant4
    """
    return self._rejectPDGs
  @rejectPDGs.setter
  def rejectPDGs( self, val ):
    self._rejectPDGs = self.makeSet(val)

  @property
  def zeroTimePDGs(self):
    """Set of PDG IDs for particles that should not be passed to Geant4 if their properTime is 0.

    The properTime of 0 indicates a documentation to add FSR to a lepton for example.
    """
    return self._zeroTimePDGs
  @zeroTimePDGs.setter
  def zeroTimePDGs(self, val):
    self._zeroTimePDGs = self.makeSet(val)

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

  @property
  def decays(self):
    """If true, add decay processes for all particles.

    Only enable when creating a physics list not based on an existing Geant4 list!
    """
    return self._decays
  @decays.setter
  def decays(self, val):
    self._decays = val

  @property
  def list(self):
    """The name of the Geant4 Physics list."""
    return self._list
  @list.setter
  def list(self, val):
    self._list = val

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
