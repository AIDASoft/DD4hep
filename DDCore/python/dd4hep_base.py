# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
from __future__ import absolute_import, unicode_literals
import cppyy
import importlib
import logging


logger = logging.getLogger(__name__)


def compileAClick(dictionary, g4=True):
  """
  We compile the DDG4 plugin on the fly if it does not exist using the AClick mechanism.

  """
  from ROOT import gInterpreter, gSystem
  import os.path
  dd4hep = os.environ['DD4hepINSTALL']
  inc = ' -I' + os.environ['ROOTSYS'] + '/include -I' + dd4hep + '/include '
  lib = ' -L' + dd4hep + '/lib -lDDCore -lDDG4 -lDDSegmentation '
  if g4:
    geant4 = os.environ['G4INSTALL']
    inc = inc + ' -I' + geant4 + '/include/Geant4 -Wno-shadow -g -O0 '
    lib = lib + ' -L' + geant4 + '/lib  -L' + geant4 + '/lib64 -lG4event -lG4tracking -lG4particles '

  gSystem.AddIncludePath(inc)
  gSystem.AddLinkedLibs(lib)
  logger.info('Loading AClick %s', dictionary)
  package_spec = importlib.util.find_spec('DDG4')
  dic = os.path.dirname(package_spec.origin) + os.sep + dictionary
  gInterpreter.ProcessLine('.L ' + dic + '+')
  from ROOT import dd4hep as module
  return module


def loaddd4hep():
  """
  Import DD4hep module from ROOT using ROOT reflection
  """
  import os
  import sys
  # Add ROOT to the python path in case it is not yet there....
  sys.path.append(os.environ['ROOTSYS'] + os.sep + 'lib')
  from ROOT import gSystem

  import platform
  if platform.system() == "Darwin":
    gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])
    os.environ['DYLD_LIBRARY_PATH'] = os.pathsep.join([os.environ['DD4HEP_LIBRARY_PATH'],
                                                       os.environ.get('DYLD_LIBRARY_PATH', '')]).strip(os.pathsep)
  result = gSystem.Load("libDDCore")
  if result < 0:
    raise Exception('dd4hep.py: Failed to load the dd4hep library libDDCore: ' + gSystem.GetErrorStr())
  from ROOT import dd4hep as module
  return module


# We are nearly there ....
name_space = __import__(__name__)


def import_namespace_item(ns, nam):
  scope = getattr(name_space, ns)
  attr = getattr(scope, nam)
  setattr(name_space, nam, attr)
  return attr


def import_root(nam):
  setattr(name_space, nam, getattr(ROOT, nam))


# ---------------------------------------------------------------------------
#
try:
  dd4hep = loaddd4hep()
  import ROOT
except Exception as X:
  import sys
  logger.error('+--%-100s--+', 100 * '-')
  logger.error('|  %-100s  |', 'Failed to load dd4hep base library:')
  logger.error('|  %-100s  |', str(X))
  logger.error('+--%-100s--+', 100 * '-')
  sys.exit(1)


class _Levels:
  def __init__(self):
    self.VERBOSE = 1
    self.DEBUG = 2
    self.INFO = 3
    self.WARNING = 4
    self.ERROR = 5
    self.FATAL = 6
    self.ALWAYS = 7


# ---------------------------------------------------------------------------
def unicode_2_string(value):
  """Turn any unicode literal into str, needed when passing to c++.

  Recursively transverses dicts, lists, sets, tuples

  :return: always a str
  """
  import ddsix as six
  if isinstance(value, (bool, float, six.integer_types)):
    value = value
  elif isinstance(value, six.string_types):
    value = str(value)
  elif isinstance(value, (list, set, tuple)):
    value = [unicode_2_string(x) for x in value]
  elif isinstance(value, dict):
    tempDict = {}
    for key, val in value.items():
      key = unicode_2_string(key)
      val = unicode_2_string(val)
      tempDict[key] = val
    value = tempDict
  return str(value)


OutputLevel = _Levels()
VERBOSE = OutputLevel.VERBOSE
DEBUG = OutputLevel.DEBUG
INFO = OutputLevel.INFO
WARNING = OutputLevel.WARNING
ERROR = OutputLevel.ERROR
FATAL = OutputLevel.FATAL


# ------------------------Generic STL stuff can be accessed using std:  -----
#
# -- e.g. Create an instance of std::vector<dd4hep::sim::Geant4Vertex*>:
#    >>> v=dd4hep.vector('dd4hep::sim::Geant4Vertex*')()
#
# ---------------------------------------------------------------------------
std = cppyy.gbl.std
std_vector = std.vector
std_list = std.list
std_map = std.map
std_pair = std.pair
# ---------------------------------------------------------------------------
core = dd4hep
cond = dd4hep.cond
tools = dd4hep.tools
align = dd4hep.align
detail = dd4hep.detail
units = imp.new_module('units')
# ---------------------------------------------------------------------------
import_namespace_item('tools', 'Evaluator')
# ---------------------------------------------------------------------------
import_namespace_item('core', 'NamedObject')
import_namespace_item('core', 'run_interpreter')
#
import_namespace_item('detail', 'interp')
# No: This inhibits the usage of native python eval!
# import_namespace_item('detail', 'eval')

# ---------------------------------------------------------------------------
# def run_interpreter(name):   detail.interp.run(name)
# def evaluator():     return eval.instance()
# def g4Evaluator():   return eval.g4instance()


# ---------------------------------------------------------------------------
def import_detail():
  import_namespace_item('detail', 'DD4hepUI')


# ---------------------------------------------------------------------------
def import_geometry():
  import_namespace_item('core', 'setPrintLevel')
  import_namespace_item('core', 'setPrintFormat')
  import_namespace_item('core', 'printLevel')
  import_namespace_item('core', 'PrintLevel')

  import_namespace_item('core', 'debug')
  import_namespace_item('core', 'info')
  import_namespace_item('core', 'warning')
  import_namespace_item('core', 'error')
  import_namespace_item('core', 'fatal')
  import_namespace_item('core', 'exception')

  import_namespace_item('core', 'Detector')
  import_namespace_item('core', 'evaluator')
  import_namespace_item('core', 'g4Evaluator')

  import_namespace_item('core', 'VolumeManager')
  import_namespace_item('core', 'OverlayedField')
  import_namespace_item('core', 'Ref_t')

  # // Objects.h
  import_namespace_item('core', 'Author')
  import_namespace_item('core', 'Header')
  import_namespace_item('core', 'Constant')
  import_namespace_item('core', 'Atom')
  import_namespace_item('core', 'Material')
  import_namespace_item('core', 'VisAttr')
  import_namespace_item('core', 'Limit')
  import_namespace_item('core', 'LimitSet')
  import_namespace_item('core', 'LimitSetObject')
  import_namespace_item('core', 'Region')
  import_namespace_item('core', 'RegionObject')
  import_namespace_item('core', 'HitCollection')

  # // Readout.h
  import_namespace_item('core', 'Segmentation')
  import_namespace_item('core', 'SegmentationObject')
  import_namespace_item('core', 'Readout')
  import_namespace_item('core', 'ReadoutObject')

  # // Alignments.h
  import_namespace_item('core', 'Alignment')
  import_namespace_item('core', 'AlignmentCondition')

  # // Conditions.h
  import_namespace_item('core', 'Condition')
  import_namespace_item('core', 'ConditionKey')

  # // DetElement.h
  import_namespace_item('core', 'World')
  import_namespace_item('core', 'DetElement')
  import_namespace_item('core', 'SensitiveDetector')

  # // Volume.h
  import_namespace_item('core', 'Volume')
  import_namespace_item('core', 'PlacedVolume')

  # // Shapes.h
  import_namespace_item('core', 'Solid')
  import_namespace_item('core', 'Box')
  import_namespace_item('core', 'HalfSpace')
  import_namespace_item('core', 'Polycone')
  import_namespace_item('core', 'ConeSegment')
  import_namespace_item('core', 'Tube')
  import_namespace_item('core', 'CutTube')
  import_namespace_item('core', 'TruncatedTube')
  import_namespace_item('core', 'EllipticalTube')
  import_namespace_item('core', 'Cone')
  import_namespace_item('core', 'Trap')
  import_namespace_item('core', 'PseudoTrap')
  import_namespace_item('core', 'Trapezoid')
  import_namespace_item('core', 'Torus')
  import_namespace_item('core', 'Sphere')
  import_namespace_item('core', 'Paraboloid')
  import_namespace_item('core', 'Hyperboloid')
  import_namespace_item('core', 'PolyhedraRegular')
  import_namespace_item('core', 'Polyhedra')
  import_namespace_item('core', 'ExtrudedPolygon')
  import_namespace_item('core', 'EightPointSolid')
  import_namespace_item('core', 'BooleanSolid')
  import_namespace_item('core', 'SubtractionSolid')
  import_namespace_item('core', 'UnionSolid')
  import_namespace_item('core', 'IntersectionSolid')


# ---------------------------------------------------------------------------
def import_tgeo():
  import_root('TGeoManager')
  import_root('TGeoNode')
  import_root('TGeoNodeMatrix')

  import_root('TGeoVolume')
  import_root('TGeoVolumeMulti')
  import_root('TGeoVolumeAssembly')

  import_root('TGeoMaterial')
  import_root('TGeoMedium')
  import_root('TGeoIsotope')
  import_root('TGeoElement')

  import_root('TGeoMatrix')
  import_root('TGeoHMatrix')
  import_root('TGeoIdentity')
  import_root('TGeoTranslation')
  import_root('TGeoRotation')
  import_root('TGeoScale')
  import_root('TGeoCombiTrans')

  import_root('TGeoShape')
  import_root('TGeoBBox')
  import_root('TGeoArb8')
  import_root('TGeoTrap')
  import_root('TGeoGtra')
  import_root('TGeoCompositeShape')
  import_root('TGeoCone')
  import_root('TGeoConeSeg')
  import_root('TGeoTube')
  import_root('TGeoTubeSeg')
  import_root('TGeoCtub')
  import_root('TGeoEltu')
  import_root('TGeoHype')
  import_root('TGeoHalfSpace')
  import_root('TGeoPara')
  import_root('TGeoParaboloid')
  import_root('TGeoPcon')
  import_root('TGeoPgon')
  import_root('TGeoScaledShape')
  import_root('TGeoShapeAssembly')
  import_root('TGeoSphere')
  import_root('TGeoTorus')
  import_root('TGeoTrd1')
  import_root('TGeoTrd2')
  import_root('TGeoXtru')


import_tgeo()
import_geometry()
import_detail()


# ---------------------------------------------------------------------------
class Logger:
  """
  Helper class to use the dd4hep printout functions from python

  \author  M.Frank
  \version 1.0
  """

  def __init__(self, name):
    "Logger constructor"
    self.name = name

  def setPrintLevel(self, level):
    "Adjust printout level of dd4hep"
    dd4hep.setPrintLevel(level)

  def always(self, msg):
    "Call dd4hep printout function with level ALWAYS"
    dd4hep.always(self.name, msg)

  def verbose(self, msg):
    "Call dd4hep printout function with level VERBOSE"
    dd4hep.verbose(self.name, msg)

  def debug(self, msg):
    "Call dd4hep printout function with level DEBUG"
    dd4hep.debug(self.name, msg)

  def info(self, msg):
    "Call dd4hep printout function with level INFO"
    dd4hep.info(self.name, msg)

  def warning(self, msg):
    "Call dd4hep printout function with level WARNING"
    dd4hep.warning(self.name, msg)

  def error(self, msg):
    "Call dd4hep printout function with level ERROR"
    dd4hep.error(self.name, msg)

  def fatal(self, msg):
    "Call dd4hep printout function with level FATAL"
    dd4hep.fatal(self.name, msg)

  def exception(self, msg):
    "Call dd4hep exception function"
    dd4hep.exception(self.name, msg)


dd4hep_logger = Logger


# ---------------------------------------------------------------------------
#
# Helper: Command line interpreter
#
# ---------------------------------------------------------------------------
class CommandLine:
  """
  Helper to ease parsing the command line.
  Any argument given in the command line is accessible
  from the object. If no value is supplied, the returned
  value is True. If the argument is not present None is returned.

  \author  M.Frank
  \version 1.0
  """
  def __init__(self, help=None):  # noqa: A002
    import sys
    self.data = {}
    help_call = help
    have_help = False
    for i in range(len(sys.argv)):
      if sys.argv[i][0] == '-':
        key = sys.argv[i][1:]
        val = True
        if i + 1 < len(sys.argv):
          v = sys.argv[i + 1]
          if v[0] != '-':
            val = v
        self.data[key] = val
        if key.upper() == 'HELP' or key.upper() == '?':
         have_help = True
    if have_help and help_call:
      help_call()

  def __getattr__(self, attr):
    if self.data.get(attr):
      return self.data.get(attr)
    return None


# ---------------------------------------------------------------------------
#
#  Import units from TGeo.
#  Calling import_units makes all the units local to the dd4hep module.
#
try:
  import_namespace_item('core', 'dd4hep_units')

  def import_units(ns=None):
    if ns is None:
      ns = name_space

    logger.debug('Importing units into namespace ' + str(ns.__name__))
    count = 0
    for nam in dir(dd4hep.dd4hep_units):
      if nam[0] != '_':
        count = count + 1
        setattr(ns, nam, getattr(core.dd4hep_units, nam))
        # setattr(ns, nam, getattr(core, nam))
    return count

except Exception as e:
  logger.warning('No units can be imported. ' + str(e))

  def import_units(ns=None):
    return 0

import_units(ns=units)
