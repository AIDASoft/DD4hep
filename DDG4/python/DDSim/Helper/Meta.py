"""Helper object for configuring the LCIO output file (meta)"""

from DDSim.Helper.ConfigHelper import ConfigHelper

class Meta( ConfigHelper ):
  """Configuration for the LCIO output file settings"""
  def __init__( self ):
    super(Meta, self).__init__()
    self._eventParameters_EXTRA = {'help': "Event parameters to write in every event. " \
                                           "Use C/F/I ids to specify parameter type. E.g parameterName/F=0.42 to set a float parameter",
                                   'nargs': '+'}
    self.eventParameters = []
    self._runNumberOffset_EXTRA = {'help': "The run number offset to write in slcio output file. "\
                                           "E.g setting it to 42 will start counting runs from 42 instead of 0"}
    self.runNumberOffset = 0
    self._eventNumberOffset_EXTRA = {'help': "The event number offset to write in slcio output file."\
                                             " E.g setting it to 42 will start counting events from 42 instead of 0"}
    self.eventNumberOffset = 0

  def parseEventParameters(self):
    """Parse the event parameters and return 3 event parameter dictionnaries, respectively for string, int and float parameters"""
    stringParameters, intParameters, floatParameters, allParameters = {}, {}, {}, []
    for p in self.eventParameters:
      parameterAndValue = p.split( "=", 1 )
      if len(parameterAndValue) != 2:
        raise SyntaxError("ERROR: Couldn't decode event parameter '%s'" %(p))
      parameterAndType = parameterAndValue[0].split( "/", 1 )
      if len(parameterAndType) != 2:
        raise SyntaxError("ERROR: Couldn't decode event parameter '%s'" %(p))
      pname = parameterAndType[0]
      ptype = parameterAndType[1]
      pvalue = parameterAndValue[1]
      if ptype.lower() not in ["c", "f", "i"]:
        raise ValueError("ERROR: Event parameter '%s' with invalid type '%s'" %(pname, ptype))
      if pname in allParameters:
        raise RuntimeError("ERROR: Event parameter '%s' specified twice" %(pname))
      if not pvalue:
        raise RuntimeError("ERROR: Event parameter '%s' has empty value" %(pname))
      allParameters.append(pname)
      print "Event parameter '%s', type '%s', value='%s'"%(pname, ptype, pvalue)
      if ptype.lower() == "c":
        stringParameters[pname] = pvalue
      elif ptype.lower() == "f":
        floatParameters[pname] = pvalue
      elif ptype.lower() == "i":
        intParameters[pname] = pvalue
    return stringParameters, intParameters, floatParameters
    