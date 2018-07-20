"""Configuration Helper for ParticleHandler"""
from SystemOfUnits import MeV, mm

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
    self._minDistToParentVertex = 2.2e-14*mm
    self._enableDetailedHitsAndParticleInfo = False

  @property
  def enableDetailedHitsAndParticleInfo(self):
    """Enable lots of printout on simulated hits and MC-truth information"""
    return self._enableDetailedHitsAndParticleInfo

  @enableDetailedHitsAndParticleInfo.setter
  def enableDetailedHitsAndParticleInfo( self, val ):
    self._enableDetailedHitsAndParticleInfo = val


  @property
  def minDistToParentVertex( self ):
    """Minimal distance between particle vertex and endpoint of parent after
    which the vertexIsNotEndpointOfParent flag is set
    """
    return self._minDistToParentVertex
  @minDistToParentVertex.setter
  def minDistToParentVertex( self, val ):
    self._minDistToParentVertex = val

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

  def setDumpDetailedParticleInfo( self, kernel, DDG4 ):
    #---- debug code from Markus for detailed dumps of hits and MC-truth assignement ------
    # Add the particle dumper to associate the MC truth
    evt = DDG4.EventAction(kernel,"Geant4ParticleDumpAction/ParticleDump")
    kernel.eventAction().adopt(evt)
    evt.enableUI()
    # Add the hit dumper BEFORE any hit truth is fixed
    evt = DDG4.EventAction(kernel,"Geant4HitDumpAction/RawDump")
    kernel.eventAction().adopt(evt)
    evt.enableUI()
    # Add the hit dumper to the event action sequence
    evt = DDG4.EventAction(kernel,"Geant4HitTruthHandler/HitTruth")
    kernel.eventAction().adopt(evt)
    evt.enableUI()
    # Add the hit dumper AFTER any hit truth is fixed. We should see the reduced track references
    evt = DDG4.EventAction(kernel,"Geant4HitDumpAction/HitDump")
    kernel.eventAction().adopt(evt)
    evt.enableUI()
