"""Helper object for physicslist properties"""

import os

from DDSim.Helper.ConfigHelper import ConfigHelper
from g4units import mm
import logging

logger = logging.getLogger(__name__)

EXCEPTION_SEVERITY_DICT = {
    "0": 0,  # FatalException
    "1": 1,  # FatalErrorInArgument
    "2": 2,  # RunMustBeAborted
    "3": 3,  # EventMustBeAborted
    "4": 4,  # JustWarning
    "5": 5,  # IgnoreTheIssue
    "FatalException": 0,
    "FatalErrorInArgument": 1,
    "RunMustBeAborted": 2,
    "EventMustBeAborted": 3,
    "JustWarning": 4,
    "IgnoreTheIssue": 5,
    }


class Physics(ConfigHelper):
  """Configuration for the PhysicsList and Monte Carlo particle selection."""

  def __init__(self):
    super(Physics, self).__init__()
    self._ETolerance = None
    self._ESeverity = None
    self._ESeverity_EXTRA = {'choices': list(EXCEPTION_SEVERITY_DICT.keys())}
    self._rangecut = 0.7 * mm
    self._list = "FTFP_BERT"
    self._decays = False
    self._pdgfile = None
    self._rejectPDGs = {1, 2, 3, 4, 5, 6,  # quarks
                        21, 23, 24, 25,  # bosons
                        1103,  # d? diquarks
                        2101, 2103, 2203,  # u? diquarks
                        3101, 3103, 3201, 3203, 3303,  # s? diquarks
                        4101, 4103, 4201, 4203, 4301, 4303, 4403,  # c? diquarks
                        5101, 5103, 5201, 5203, 5301, 5303, 5401, 5403, 5503}  # b? diquarks
    self._zeroTimePDGs = {11, 13, 15, 17}
    self._decayByGeant = set()
    self._alternativeDecayStatuses = set()
    self._alternativeStableStatuses = set()
    self._userFunctions = []
    self._closeProperties()
    Physics.__doc__ += "\n\n" + self.setupUserPhysics.__doc__

  @property
  def rejectPDGs(self):
    """Set of PDG IDs that will not be passed from the input record to Geant4.

    Quarks, gluons and W's Z's etc should not be treated by Geant4
    """
    return self._rejectPDGs

  @rejectPDGs.setter
  def rejectPDGs(self, val):
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
  def decayByGeant(self):
    """Set of PDG IDs for particles that should be decayed according to the lifetime.

    Lifetimes are either configured in Geant4 or particle.tbl instead of their pre-assigned decay time.

    """
    return self._decayByGeant

  @decayByGeant.setter
  def decayByGeant(self, val):
    self._decayByGeant = self.makeSet(val)

  @property
  def alternativeDecayStatuses(self):
    """Set of Generator Statuses that are used to mark unstable particles that should decay inside of Geant4.
    """
    return self._alternativeDecayStatuses

  @alternativeDecayStatuses.setter
  def alternativeDecayStatuses(self, val):
    self._alternativeDecayStatuses = self.makeSet(val)

  @property
  def alternativeStableStatuses(self):
    """Set of Generator Statuses that are used to mark stable particles that should be forwarded to Geant4.
    """
    return self._alternativeStableStatuses

  @alternativeStableStatuses.setter
  def alternativeStableStatuses(self, val):
    self._alternativeStableStatuses = self.makeSet(val)

  @property
  def rangecut(self):
    """ The global geant4 rangecut for secondary production

    Default is 0.7 mm as is the case in geant4 10

    To disable this plugin and be absolutely sure to use the Geant4 default range cut use "None"

    Set printlevel to DEBUG to see a printout of all range cuts,
    but this only works if range cut is not "None"
    """
    return self._rangecut

  @rangecut.setter
  def rangecut(self, val):
    if val is None:
      self._rangecut = None
      return
    if isinstance(val, str):
      if val == "None":
        self._rangecut = None
        return
    self._rangecut = val

  @property
  def pdgfile(self):
    """Location of particle.tbl file containing extra particles and their lifetime information

    For example in $DD4HEP/examples/DDG4/examples/particle.tbl

    This is a vital setting if you want to simulate secondary vertices or pre-assigned decays or both. Geant4 has to
    know about all particles with non-negligible lifetime. Use this setting together with alternativeStableStatuses and
    alternativeDecayStatuses, to configure the simulation to suit your MCGenerator file.
    """
    return self._pdgfile

  @pdgfile.setter
  def pdgfile(self, val):
    if not val:
      self._pdgfile = None
      return
    if not os.path.exists(val):
      raise RuntimeError("PDGFile: %s not found" % os.path.abspath(val))
    self._pdgfile = os.path.abspath(val)

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
  def list(self):  # noqa: A003
    """The name of the Geant4 Physics list."""
    return self._list

  @list.setter
  def list(self, val):  # noqa: A003
    self._list = val

  @property
  def ETolerance(self):
    """Configure the tolerance for the mass check of dynamic Particles, a.k.a, SetKETolerance
    """
    return self._ETolerance

  @ETolerance.setter
  def ETolerance(self, val):
    self._ETolerance = val

  @property
  def ESeverity(self):
    """Configure the severity for the mass check of dynamic Particles, a.k.a, SetKETolerance
    """
    return self._ESeverity

  @ESeverity.setter
  def ESeverity(self, val):
    if val is None:
      self._ESeverity = val
      return
    val = EXCEPTION_SEVERITY_DICT[str(val)]
    self._ESeverity = val

  def setupPhysics(self, kernel, name=None):
    seq = kernel.physicsList()
    seq.extends = name if name is not None else self.list
    seq.decays = self.decays
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
      rg = PhysicsList(kernel, 'Geant4DefaultRangeCut/GlobalRangeCut')
      rg.enableUI()
      seq.adopt(rg)
      rg.RangeCut = self.rangecut

    # Add setting of KETolerance
    if self._ETolerance is not None or self._ESeverity is not None:
      seq = kernel.physicsList()
      rg = PhysicsList(kernel, 'Geant4SetKETolerance/KETolerance')
      rg.enableUI()
      seq.adopt(rg)
      if self._ETolerance is not None:
        rg.Tolerance = self._ETolerance
      if self._ESeverity is not None:
        rg.Severity = self._ESeverity

    for func in self._userFunctions:
      try:
        func(kernel)
      except Exception as e:
        logger.error("Exception in UserFunction: %r", e)
        raise RuntimeError("Exception in UserFunction: %r" % e)

    return seq

  def setupUserPhysics(self, userFunction):
    """To load arbitrary plugins, add a function to be executed.

    The function must take the DDG4.Kernel() object as the only argument.

    For example, add a function definition and the call to a steering file::

      def setupCerenkov(kernel):
        from DDG4 import PhysicsList
        seq = kernel.physicsList()
        cerenkov = PhysicsList(kernel, 'Geant4CerenkovPhysics/CerenkovPhys')
        cerenkov.MaxNumPhotonsPerStep = 10
        cerenkov.MaxBetaChangePerStep = 10.0
        cerenkov.TrackSecondariesFirst = True
        cerenkov.VerboseLevel = 2
        cerenkov.enableUI()
        seq.adopt(cerenkov)
        ph = PhysicsList(kernel, 'Geant4OpticalPhotonPhysics/OpticalGammaPhys')
        ph.addParticleConstructor('G4OpticalPhoton')
        ph.VerboseLevel = 2
        ph.enableUI()
        seq.adopt(ph)
        return None

      SIM.physics.setupUserPhysics(setupCerenkov)

    # End of example
    """
    self._userFunctions.append(userFunction)
