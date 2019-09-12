"""Helper object for configuring the LCIO output file (meta)"""

from __future__ import absolute_import, unicode_literals
from DDSim.Helper.ConfigHelper import ConfigHelper
import datetime
import os
import logging
import ddsix as six
from io import open

logger = logging.getLogger(__name__)


class Meta(ConfigHelper):
  """Configuration for the LCIO output file settings"""

  def __init__(self):
    super(Meta, self).__init__()
    self._eventParameters_EXTRA = {'help': "Event parameters to write in every event. "
                                           "Use C/F/I ids to specify parameter type. "
                                           "E.g parameterName/F=0.42 to set a float parameter",
                                   'nargs': '+'}
    self.eventParameters = []
    self._runNumberOffset_EXTRA = {'help': "The run number offset to write in slcio output file. "
                                           "E.g setting it to 42 will start counting runs from 42 instead of 0"}
    self.runNumberOffset = 0
    self._eventNumberOffset_EXTRA = {'help': "The event number offset to write in slcio output file."
                                             " E.g setting it to 42 will start counting events from 42 instead of 0"}
    self.eventNumberOffset = 0

  def parseEventParameters(self):
    """
    Parse the event parameters and return 3 event parameter dictionnaries, respectively
    for string, int and float parameters
    """
    stringParameters, intParameters, floatParameters, allParameters = {}, {}, {}, []
    for p in self.eventParameters:
      parameterAndValue = p.split("=", 1)
      if len(parameterAndValue) != 2:
        raise SyntaxError("ERROR: Couldn't decode event parameter '%s'" % (p))
      parameterAndType = parameterAndValue[0].split("/", 1)
      if len(parameterAndType) != 2:
        raise SyntaxError("ERROR: Couldn't decode event parameter '%s'" % (p))
      pname = parameterAndType[0]
      ptype = parameterAndType[1]
      pvalue = parameterAndValue[1]
      if ptype.lower() not in ["c", "f", "i"]:
        raise ValueError("ERROR: Event parameter '%s' with invalid type '%s'" % (pname, ptype))
      if pname in allParameters:
        raise RuntimeError("ERROR: Event parameter '%s' specified twice" % (pname))
      if not pvalue:
        raise RuntimeError("ERROR: Event parameter '%s' has empty value" % (pname))
      allParameters.append(pname)
      logger.info("Event parameter '%s', type '%s', value='%s'" % (pname, ptype, pvalue))
      if ptype.lower() == "c":
        stringParameters[pname] = pvalue
      elif ptype.lower() == "f":
        floatParameters[pname] = pvalue
      elif ptype.lower() == "i":
        intParameters[pname] = pvalue
    return stringParameters, intParameters, floatParameters

  @staticmethod
  def addParametersToRunHeader(sim):
    """add the parameters to the (lcio) run Header"""
    runHeader = {}
    parameters = vars(sim)
    for parName, parameter in six.iteritems(parameters):
      if isinstance(parameter, ConfigHelper):
        options = parameter.getOptions()
        for opt, optionsDict in six.iteritems(options):
          runHeader["%s.%s" % (parName, opt)] = str(optionsDict['default'])
      else:
        runHeader[parName] = str(parameter)

    # steeringFile content
    if sim.steeringFile and os.path.exists(sim.steeringFile) and os.path.isfile(sim.steeringFile):
      with open(sim.steeringFile) as sFile:
        runHeader["SteeringFileContent"] = sFile.read()

    # macroFile content
    if sim.macroFile and os.path.exists(sim.macroFile) and os.path.isfile(sim.macroFile):
      with open(sim.macroFile) as mFile:
        runHeader["MacroFileContent"] = mFile.read()

    # add command line
    if sim._argv:
      runHeader["CommandLine"] = " ".join(sim._argv)

    # add current working directory (where we call from)
    runHeader["WorkingDirectory"] = os.getcwd()

    # ILCSoft, LCGEo location from environment variables, names from init_ilcsoft.sh
    runHeader["ILCSoft_location"] = os.environ.get("ILCSOFT", "Unknown")
    runHeader["lcgeo_location"] = os.environ.get("lcgeo_DIR", "Unknown")

    # add date
    runHeader["DateUTC"] = str(datetime.datetime.utcnow()) + " UTC"

    # add User
    import getpass
    runHeader["User"] = getpass.getuser()

    return runHeader
