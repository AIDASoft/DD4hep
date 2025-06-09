"""Class for output file configuration"""
import logging

from DDSim.Helper.ConfigHelper import ConfigHelper

logger = logging.getLogger(__name__)

#: True if DD4hep was built with LCIO
DD4HEP_USE_LCIO = "@DD4HEP_USE_LCIO@" != "OFF"
#: True if DD4hep was built with EDM4hep
DD4HEP_USE_EDM4HEP = "@DD4HEP_USE_EDM4HEP@" != "OFF"


def defaultOutputFile():
  if DD4HEP_USE_LCIO and not DD4HEP_USE_EDM4HEP:
    return "ddsimOutput.slcio"
  return "ddsimOutput.root"


class OutputConfig(ConfigHelper):
  """Configuration for Output Files."""

  def __init__(self):
    super(OutputConfig, self).__init__()
    self._userPlugin = None
    self._forceLCIO = False
    self._forceEDM4HEP = False
    self._forceDD4HEP = False
    # no closeProperties, allow custom ones for userPlugin configuration

  def _checkConsistency(self):
    """Raise error if more than one force flag is true."""
    if self._forceLCIO + self._forceEDM4HEP + self._forceDD4HEP > 1:
      raise RuntimeError(f"OutputConfig error: More than one force flag enabled: LCIO({self._forceLCIO}),"
                         f" EDM4HEP({self._forceEDM4HEP}), DD4HEP({self._forceDD4HEP})")

  @property
  def forceLCIO(self):
    """Use the LCIO output plugin regardless of outputfilename."""
    return self._forceLCIO

  @forceLCIO.setter
  def forceLCIO(self, val):
    self._forceLCIO = self.makeBool(val)
    if self._forceLCIO:
      if not DD4HEP_USE_LCIO:
        raise RuntimeError("OutputConfig error: forceLCIO requested, but LCIO not available!")
      self._checkConsistency()

  @property
  def forceEDM4HEP(self):
    """Use the EDM4HEP output plugin regardless of outputfilename."""
    return self._forceEDM4HEP

  @forceEDM4HEP.setter
  def forceEDM4HEP(self, val):
    self._forceEDM4HEP = self.makeBool(val)
    if self._forceEDM4HEP:
      if not DD4HEP_USE_EDM4HEP:
        raise RuntimeError("OutputConfig error: forceEDM4HEP requested, but EDM4HEP not available!")
      self._checkConsistency()

  @property
  def forceDD4HEP(self):
    """Use the DD4HEP output plugin regardless of outputfilename."""
    return self._forceDD4HEP

  @forceDD4HEP.setter
  def forceDD4HEP(self, val):
    self._forceDD4HEP = self.makeBool(val)
    if self._forceDD4HEP:
      self._checkConsistency()

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
        output = dd.outputFile
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

  def initialize(self, dd4hepsimulation, geant4):
    """Configure the output file and plugin."""
    if callable(self._userPlugin):
      logger.info("++++ Setting up UserPlugin for Output ++++")
      return self._userPlugin(dd4hepsimulation)

    if self.forceLCIO:
      return self._configureLCIO(dd4hepsimulation, geant4)

    if self.forceEDM4HEP:
      return self._configureEDM4HEP(dd4hepsimulation, geant4)

    if self.forceDD4HEP:
      return self._configureDD4HEP(dd4hepsimulation, geant4)

    if dd4hepsimulation.outputFile.endswith(".slcio"):
      return self._configureLCIO(dd4hepsimulation, geant4)

    if dd4hepsimulation.outputFile.endswith(".root") and DD4HEP_USE_EDM4HEP:
      return self._configureEDM4HEP(dd4hepsimulation, geant4)

    if dd4hepsimulation.outputFile.endswith(".root"):
      return self._configureDD4HEP(dd4hepsimulation, geant4)

  def _configureLCIO(self, dds, geant4):
    if not DD4HEP_USE_LCIO:
      raise RuntimeError("DD4HEP was not build wiht LCIO support: please change output format %s" % dds.outputFile)
    logger.info("++++ Setting up LCIO Output ++++")
    lcOut = geant4.setupLCIOOutput('LcioOutput', dds.outputFile)
    lcOut.RunHeader = dds.meta.addParametersToRunHeader(dds)
    eventPars = dds.meta.parseEventParameters()
    lcOut.EventParametersString, lcOut.EventParametersInt, lcOut.EventParametersFloat = eventPars
    runPars = dds.meta.parseRunParameters()
    lcOut.RunParametersString, lcOut.RunParametersInt, lcOut.RunParametersFloat = runPars
    lcOut.RunNumberOffset = dds.meta.runNumberOffset if dds.meta.runNumberOffset > 0 else 0
    lcOut.EventNumberOffset = dds.meta.eventNumberOffset if dds.meta.eventNumberOffset > 0 else 0
    return

  def _configureEDM4HEP(self, dds, geant4):
    logger.info("++++ Setting up EDM4hep ROOT Output ++++")
    e4Out = geant4.setupEDM4hepOutput('EDM4hepOutput', dds.outputFile)
    eventPars = dds.meta.parseEventParameters()
    e4Out.RunHeader = dds.meta.addParametersToRunHeader(dds)
    e4Out.EventParametersString, e4Out.EventParametersInt, e4Out.EventParametersFloat = eventPars
    runPars = dds.meta.parseRunParameters()
    e4Out.RunParametersString, e4Out.RunParametersInt, e4Out.RunParametersFloat = runPars
    e4Out.RunNumberOffset = dds.meta.runNumberOffset if dds.meta.runNumberOffset > 0 else 0
    e4Out.EventNumberOffset = dds.meta.eventNumberOffset if dds.meta.eventNumberOffset > 0 else 0
    return

  def _configureDD4HEP(self, dds, geant4):
    logger.info("++++ Setting up DD4hep's ROOT Output ++++")
    geant4.setupROOTOutput('RootOutput', dds.outputFile)
    return
