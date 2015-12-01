"""Configuration Helper for ParticleHandler"""
from SystemOfUnits import MeV

from DDSim.Helper.ConfigHelper import ConfigHelper

class ParticleHandler( ConfigHelper ):
  """Configuration for the Particle Handler/ MCTruth treatment"""
  def __init__( self ):
    super(ParticleHandler, self).__init__()
    self._saveProcesses = ['Decay']
    self._minimalKineticEnergy = 1*MeV
    self._keepAllParticles = False
    self._printEndTracking = False
    self._printStartTracking = False

  @property
  def saveProcesses(self):
    """List of processes to save, on command line give as whitespace separated string in quotation marks"""
    return self._saveProcesses
  @saveProcesses.setter
  def saveProcesses(self, stringVal):
    self._saveProcesses = ConfigHelper.makeList( stringVal )

  @property
  def minimalKineticEnergy(self):
    """MinimalKineticEnergy to store particles created in the tracking region"""
    return self._minimalKineticEnergy
  @minimalKineticEnergy.setter
  def minimalKineticEnergy( self, val ):
    self._minimalKineticEnergy = val

  @property
  def keepAllParticles( self ):
    """ Keep all created particles """
    return self._keepAllParticles
  @keepAllParticles.setter
  def keepAllParticles( self, val ):
    self._keepAllParticles = val

  @property
  def printStartTracking( self ):
    """ Printout at Start of Tracking """
    return self._printStartTracking
  @printStartTracking.setter
  def printStartTracking( self, val ):
    self._printEndTracking = val

  @property
  def printEndTracking( self ):
    """ Printout at End of Tracking """
    return self._printEndTracking
  @printEndTracking.setter
  def printEndTracking( self, val ):
    self._printEndTracking = val
