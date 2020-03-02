"""Helper object for hepmc3 input control"""

from __future__ import absolute_import, unicode_literals
from DDSim.Helper.Input import Input


class HepMC3(Input):
  """Configuration for the hepmc3 InputFiles"""

  def __init__(self):
    super(HepMC3, self).__init__()
    self._parameters["Flow1"] = "flow1"
    self._parameters["Flow2"] = "flow2"
    # this option will evaluate to True if the HEPMC3 plugin was build
    self._useHepMC3 = ("@DD4HEP_USE_HEPMC3@" != "OFF")

  @property
  def useHepMC3(self):
    """Set to false if the input should be opened with the hepmc2 ascii reader.

    If ``True`` a  '.hepmc' file will be opened with the HEPMC3 Reader Factory.

    Defaults to true if DD4hep was build with HEPMC3 support.
    """
    return self._useHepMC3

  @useHepMC3.setter
  def useHepMC3(self, val):
    self._useHepMC3 = self.makeBool(val)

  @property
  def Flow1(self):
    """Set the name of the attribute contraining color flow information index 0."""
    return self._parameters["Flow1"]

  @Flow1.setter
  def Flow1(self, val):
    self._parameters["Flow1"] = val

  @property
  def Flow2(self):
    """Set the name of the attribute contraining color flow information index 1."""
    return self._parameters["Flow2"]

  @Flow2.setter
  def Flow2(self, val):
    self._parameters["Flow2"] = val
