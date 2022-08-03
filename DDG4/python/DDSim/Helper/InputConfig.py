"""Class for input plugin configuration"""

from __future__ import absolute_import, unicode_literals
from DDSim.Helper.ConfigHelper import ConfigHelper


class InputConfig(ConfigHelper):
  """Configuration for Input Files."""

  def __init__(self):
    super(InputConfig, self).__init__()
    self._userPlugin = []

  @property
  def userInputPlugin(self):
    """Set one or more functions to configure input steps.

    The functions must take a ``DD4hepSimulation`` object as their only argument and return the created generatorAction
    ``gen`` (for example).

    For example one can add this to the ddsim steering file:

      def exampleUserPlugin(dd4hepSimulation):
        '''Example code for user created plugin.

        :param DD4hepSimulation dd4hepSimulation: The DD4hepSimulation instance, so all parameters can be accessed
        :return: GeneratorAction
        '''
        from DDG4 import GeneratorAction, Kernel
        # Geant4InputAction is the type of plugin, Cry1 just an identifier
        gen = GeneratorAction(Kernel(), 'Geant4InputAction/Cry1' , True)
        # CRYEventReader is the actual plugin, steeringFile its constructor parameter
        gen.Input = 'CRYEventReader|' + 'steeringFile'
        # we can give a dictionary of Parameters that has to be interpreted by the setParameters function of the plugin
        gen.Parameters = {'DataFilePath': '/path/to/files/data'}
        gen.enableUI()
        return gen

      SIM.inputConfig.userInputPlugin = exampleUserPlugin

    Repeat function definition and assignment to add multiple input steps

    """
    return self._userPlugin

  @userInputPlugin.setter
  def userInputPlugin(self, userInputPluginConfig):
    if not userInputPluginConfig:
      return

    if isinstance(userInputPluginConfig, list):
      if not all(callable(func) for func in userInputPluginConfig):
        raise RuntimeError("Some provided userPlugins are not a callable function")
      self._userPlugin = userInputPluginConfig
      return

    if not callable(userInputPluginConfig):
      raise RuntimeError("The provided userPlugin is not a callable function: %s, %s" % (type(userInputPluginConfig),
                                                                                         repr(userInputPluginConfig)))
    self._userPlugin.append(userInputPluginConfig)
