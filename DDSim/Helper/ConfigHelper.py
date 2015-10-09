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
        finalVars[var] = (val,'')

    # now get things defined with @property
    props = [(p, getattr(type(self),p)) for p in dir(type(self)) if isinstance(getattr(type(self),p),property)]
    for propName, prop in props:
      finalVars[propName] = (getattr(self, propName), prop.__doc__)

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
  def listifyString( stringVal, sep=" "):
    """returns a list from a string separated by sep"""
    if isinstance( stringVal, list ):
      return stringVal
    else:
      return stringVal.split(sep)
