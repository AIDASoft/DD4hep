"""Helper object for configuration of Geant4 commands to run during different phases."""

from DDSim.Helper.ConfigHelper import ConfigHelper


class UI(ConfigHelper):
  """Configuration for setting commands to run during different phases.

  In this section, one can configure commands that should be run during the different phases of the Geant4 execution.

  1. Configuration
  2. Initialization
  3. Pre Run
  4. Post Run
  5. Terminate / Finalization

  For example, one can add

  >>> SIM.ui.commandsConfigure = ['/physics_lists/em/SyncRadiation true']

  Further details should be taken from the Geant4 documentation.
  """

  def __init__(self):
    super(UI, self).__init__()

    self._commandsConfigure = []
    self._commandsInitialize = []
    self._commandsPostRun = []
    self._commandsPreRun = []
    self._commandsTerminate = []

  @property
  def commandsConfigure(self):
    """List of UI commands to run during the 'Configure' phase."""
    return self._commandsConfigure

  @commandsConfigure.setter
  def commandsConfigure(self, val):
    self._commandsConfigure = self.makeList(val)

  @property
  def commandsInitialize(self):
    """List of UI commands to run during the 'Initialize' phase."""
    return self._commandsInitialize

  @commandsInitialize.setter
  def commandsInitialize(self, val):
    self._commandsInitialize = self.makeList(val)

  @property
  def commandsPostRun(self):
    """List of UI commands to run during the 'PostRun' phase."""
    return self._commandsPostRun

  @commandsPostRun.setter
  def commandsPostRun(self, val):
    self._commandsPostRun = self.makeList(val)

  @property
  def commandsPreRun(self):
    """List of UI commands to run during the 'PreRun' phase."""
    return self._commandsPreRun

  @commandsPreRun.setter
  def commandsPreRun(self, val):
    self._commandsPreRun = self.makeList(val)

  @property
  def commandsTerminate(self):
    """List of UI commands to run during the 'Terminate' phase."""
    return self._commandsTerminate

  @commandsTerminate.setter
  def commandsTerminate(self, val):
    self._commandsTerminate = self.makeList(val)
