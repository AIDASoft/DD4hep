"""

Helper object to identify configuration parameters so we can easily overwrite
them via command line magic or via the steering file

"""

from pprint import pprint

class ConfigHelper( object ):
  """Base class for configuration helper"""
  def __init__( self ):
    pass

  def getOptions(self):
    finalVars = {}

    # get all direct members not starting with underscore
    allVars = vars(self)
    for var,val in allVars.iteritems():
      if not var.startswith('_'):
        helpName = "_%s_HELP" % var
        optName = "_%s_OPTIONS" % var
        doc = getattr(self, helpName) if hasattr(self, helpName) else ''
        choices = getattr(self, optName) if hasattr(self, optName) else None
        finalVars[var] = (val, doc, choices)

    # now get things defined with @property
    props = [(p, getattr(type(self),p)) for p in dir(type(self)) if isinstance(getattr(type(self),p),property)]
    for propName, prop in props:
      optName =  "_%s_OPTIONS" % propName
      choices = getattr(self, optName) if hasattr(self, optName) else None
      finalVars[propName] = (getattr(self, propName), prop.__doc__, choices)

    return finalVars

  def __repr__(self):
    return self.printOptions()

  def printOptions( self ):
    """print all paramters"""
    options = []
    for opt,val in self.getOptions().iteritems():
      options.append("\n\t'%s': '%s'" % (opt, val[0]))
    return "".join(options)

  def setOption( self, name, val ):
    """ set the attribute name to val """
    setattr(self, name, val)

  @staticmethod
  def makeList( stringVal, sep=" "):
    """returns a list from a string separated by sep"""
    if not stringVal:
      return []
    if isinstance( stringVal, list ):
      return stringVal
    else:
      return stringVal.split(sep)

  @staticmethod
  def makeTuple( val ):
    """ returns a tuple of the string, separators are space or comma """
    myTuple = None
    if isinstance( val, tuple ):
      myTuple = val
    if isinstance( val, list ):
      myTuple = tuple(val)
    if isinstance( val, basestring ):
      sep = ',' if ',' in val else ' '
      myTuple = tuple([ _.strip("(), ") for _ in val.split(sep) ])
    if myTuple is None:
      raise RuntimeError( "Cannot parse input value %s" % val )
    return myTuple

  @staticmethod
  def makeBool( val ):
    """check if val is a bool or a string of true/false, otherwise raise exception"""
    if isinstance(val, bool):
      return val
    elif isinstance(val, basestring):
      if val.lower() == 'true':
        return True
      elif val.lower() == 'false':
        return False
    raise RuntimeError( val )
