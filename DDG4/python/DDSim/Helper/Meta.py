"""Helper object for configuring the LCIO output file (meta)"""

from DDSim.Helper.ConfigHelper import ConfigHelper
import datetime
import io
import os
import logging

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
    self._runParameters_EXTRA = {'help': "Run parameters to write in the run header. "
                                         "Use C/F/I ids to specify parameter type. "
                                         "E.g parameterName/F=0.42 to set a float parameter",
                                 'nargs': '+'}
    self.runParameters = []
    self._runNumberOffset_EXTRA = {'help': "The run number offset to write in slcio output file. "
                                           "E.g setting it to 42 will start counting runs from 42 instead of 0",
                                   'type': int}
    self.runNumberOffset = 0
    self._eventNumberOffset_EXTRA = {'help': "The event number offset to write in slcio output file."
                                             " E.g setting it to 42 will start counting events from 42 instead of 0",
                                     'type': int}
    self.eventNumberOffset = 0
    # no closeProperties, allow adding arbitrary information to runHeader

  def parseMetaParameters(self, parameterType="event"):
    """
    Parse the event parameters and return 3 event parameter dictionaries, respectively
    for string, int and float parameters
    """
    stringParameters, intParameters, floatParameters, allParameters = {}, {}, {}, []
    metaParameters = self.eventParameters if parameterType == "event" else self.runParameters
    for p in metaParameters:
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
    for parName, parameter in parameters.items():
      if isinstance(parameter, ConfigHelper):
        options = parameter.getOptions()
        for opt, optionsDict in options.items():
          runHeader["%s.%s" % (parName, opt)] = str(optionsDict['default'])
      else:
        runHeader[parName] = str(parameter)

    # steeringFile content
    if sim.steeringFile and os.path.exists(sim.steeringFile) and os.path.isfile(sim.steeringFile):
      with io.open(sim.steeringFile) as sFile:
        runHeader["SteeringFileContent"] = sFile.read()

    # macroFile content
    if sim.macroFile and os.path.exists(sim.macroFile) and os.path.isfile(sim.macroFile):
      with io.open(sim.macroFile) as mFile:
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
    try:
        runHeader["User"] = getpass.getuser()
    except KeyError:
        runHeader["User"] = str(os.getuid())

    return runHeader
