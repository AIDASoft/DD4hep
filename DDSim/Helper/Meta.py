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


