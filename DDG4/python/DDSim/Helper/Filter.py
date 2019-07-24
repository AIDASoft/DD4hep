"""Helper object for Filters

Later the parameter dictionary is used to instantiate the filter object
The default filters are a GeantinoRejector and a 1keV minimum energy cut

"""

from __future__ import absolute_import, unicode_literals
from DDSim.Helper.ConfigHelper import ConfigHelper
from g4units import keV
import logging

logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

class Filter( ConfigHelper ):
  """Configuration for sensitive detector filters

  Set the default filter for tracker or caliromter
  >>> SIM.filter.tracker = "edep1kev"
  >>> SIM.filter.calo = ""

  Assign a filter to a sensitive detector via pattern matching
  >>> SIM.filter.mapDetFilter['FTD'] = "edep1kev"

  Or more than one filter:
  >>> SIM.filter.mapDetFilter['FTD'] = ["edep1kev", "geantino"]

  Don't use the default filter or anything else:
  >>> SIM.filter.mapDetFilter['TPC'] = None ## or "" or []

  Create a custom filter. The dictionary is used to instantiate the filter later on
  >>> SIM.filter.filters['edep3kev'] = dict(name="EnergyDepositMinimumCut/3keV", parameter={"Cut": 3.0*keV} )

  """
  def __init__( self ):
    super(Filter, self).__init__()
    self._mapDetFilter = {}
    self._tracker = "edep1kev"
    self._calo    = "edep0"
    self._filters = {}
    self._createDefaultFilters()

  @property
  def tracker( self ):
    """ default filter for tracking sensitive detectors; this is applied if no other filter is used for a tracker"""
    return self._tracker
  @tracker.setter
  def tracker( self, val ):
    self._tracker = val

  @property
  def calo( self ):
    """ default filter for calorimeter sensitive detectors; this is applied if no other filter is used for a calorimeter """
    return self._calo
  @calo.setter
  def calo( self, val ):
    self._calo = val


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
    raise RuntimeError("Commandline setting of filters is not supported, use a steeringFile: %s " % val )


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

  def resetFilter( self ):
    """ remove all filters """
    self._filters = {}

  def _createDefaultFilters( self ):
    """ create the map with the default filters """
    self.filters["geantino"]= dict( name="GeantinoRejectFilter/GeantinoRejector",
                                    parameter={} )

    self.filters["edep1kev"] = dict( name="EnergyDepositMinimumCut",
                                     parameter={"Cut": 1.0*keV} )

    self.filters["edep0"] = dict( name="EnergyDepositMinimumCut/Cut0",
                                  parameter={"Cut": 0.0} )


  def __makeMapDetList( self ):
    """ create the values of the mapDetFilters a list of filters """
    for pattern, filters in self._mapDetFilter.iteritems():
      self._mapDetFilter[pattern] = ConfigHelper.makeList(filters)


  def setupFilters( self, kernel):
    """ attach all filters to the kernel """
    import DDG4
    setOfFilters = set()

    for name, filt in self.filters.iteritems():
      setOfFilters.add(name)
      ddFilt = DDG4.Filter(kernel,filt['name'])
      for para, value in filt['parameter'].iteritems():
        setattr( ddFilt, para, value )
      kernel.registerGlobalFilter(ddFilt)
      filt['filter'] = ddFilt

    from itertools import chain
    listOfFilters = []
    for val in self.mapDetFilter.values():
      listOfFilters += ConfigHelper.makeList(val)
    requestedFilter = set(chain( ConfigHelper.makeList(self.tracker), ConfigHelper.makeList(self.calo), listOfFilters))
    logger.info("ReqFilt %s", requestedFilter)
    if requestedFilter - setOfFilters:
      raise RuntimeError(" Filter(s) '%s' are not registered!" %  str(requestedFilter - setOfFilters) )

  def applyFilters( self, seq, det, defaultFilter=None):
    """apply the filters to to the sensitive detector

    :param seq: sequence object returned when creating sensitive detector
    :param det: sensitive detector name
    :returns: None
    """
    self.__makeMapDetList()
    foundFilter=False
    for pattern, filts in self.mapDetFilter.iteritems():
      if pattern.lower() in det.lower():
        foundFilter = True
        for filt in filts:
          logger.info("Adding filter '%s' matched with '%s' to sensitive detector for '%s' " %( filt, pattern, det ))
          seq.add( self.filters[filt]['filter'] )

    if not foundFilter and defaultFilter:
      seq.add( self.filters[defaultFilter]['filter'] )
