"""Class for output file configuration"""

from __future__ import absolute_import, unicode_literals
from DDSim.Helper.ConfigHelper import ConfigHelper


class OutputConfig(ConfigHelper):
  """Configuration for Output Files."""

  def __init__(self):
    super(OutputConfig, self).__init__()
    self._userPlugin = None

  @property
  def userOutputPlugin(self):
    """Set a function to configure the outputFile.

    The function must take a ``DD4hepSimulation`` object as its only argument and return ``None``.

    For example one can add this to the ddsim steering file:

      def exampleUserPlugin(dd4hepSimulation):
        '''Example code for user created plugin.

        :param DD4hepSimulation dd4hepSimulation: The DD4hepSimulation instance, so all parameters can be accessed
        :return: None
        '''
        from DDG4 import EventAction, Kernel
        dd = dd4hepSimulation  # just shorter variable name
        evt_root = EventAction(Kernel(), 'Geant4Output2ROOT/' + dd.outputFile, True)
        evt_root.HandleMCTruth = True or False
        evt_root.Control = True
        if not dd.outputFile.endswith(dd.outputConfig.myExtension):
          output = dd.outputFile + dd.outputConfig.myExtension
        evt_root.Output = output
        evt_root.enableUI()
        Kernel().eventAction().add(evt_root)
        return None

      SIM.outputConfig.userOutputPlugin = exampleUserPlugin
      # arbitrary options can be created and set via the steering file or command line
      SIM.outputConfig.myExtension = '.csv'
    """
    return self._userPlugin

  @userOutputPlugin.setter
  def userOutputPlugin(self, userOutputPluginConfig):
    if userOutputPluginConfig is None:
      return
    if not callable(userOutputPluginConfig):
      raise RuntimeError("The provided userPlugin is not a callable function.")
    self._userPlugin = userOutputPluginConfig
