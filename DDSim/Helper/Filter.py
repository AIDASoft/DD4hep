"""Helper object for Filters

>>> SIM = DD4hepSimulation()
>>> SIM.filter.mapDetFilter['FTD'] = "edep3keV"
>>> SIM.filter.filters['edep3keV'] = dict(name="EnergyDepositMinimumCut/3keV", parameter={"Cut": 3.0*keV} )


Later the paramter dictionary is used to instantiate the filter object
The default filters are a GeantinoRejector and a 1keV minimum energy cut

"""

from DDSim.Helper.ConfigHelper import ConfigHelper
from SystemOfUnits import keV

class Filter( ConfigHelper ):
  """Filter for sensitive detector actions"""
  def __init__( self ):
    super(Filter, self).__init__()
    self._mapDetFilter = {}
    self._tracker = "edep"
    self._calo    = None
    self._filters = {}
    self._createDefaultFilters()

  @property
  def filters( self ):
    """ list of filter objects: map between name and parameter dictionary """
    return self._filters
  @filters.setter
  def filters( self, val ):
    if isinstance(val, dict):
      self._filters.update(val)
      return
    ## 
    raise RuntimeError("Commandline setting of filters is not supported, use a steeringFile:%s " % val )


  @property
  def mapDetFilter( self ):
    """ a map between patterns and filter objects, using patterns to attach filters to sensitive detector """
    return self._mapDetFilter
  @mapDetFilter.setter
  def mapDetFilter( self, val ):
    if isinstance(val, dict):
      self._mapDetFilter.update(val)
      return

    if isinstance(val, basestring):
      vals = val.split(" ")
    elif isinstance( val, list ):
      vals = val
    if len(vals)%2 != 0:
      raise RuntimeError("Not enough parameters for mapDetFilter")
    for index in xrange(0,len(vals),2):
      self._mapDetFilter[vals[index]] = vals[index+1]

    self._mapDetFilter = val


  def _createDefaultFilters( self ):
    """ create the map with the default filters """
    self.filters["geantino"]= dict( name="GeantinoRejectFilter/GeantinoRejector",
                                    parameter={} )

    self.filters["edep1kev"] = dict( name="EnergyDepositMinimumCut",
                                     parameter={"Cut": 1.0*keV} )


  def setupFilters( self, kernel):
    """ attach all filters to the kernel """
    import DDG4

    for _pattern, filt in self.filters.iteritems():
      ddFilt = DDG4.Filter(kernel,filt['name'])
      for para, value in filt['parameter'].iteritems():
        setattr( ddFilt, para, value )
      kernel.registerGlobalFilter(ddFilt)
      filt['filter'] = ddFilt
