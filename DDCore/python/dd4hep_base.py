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
import imp
import logging

logger = logging.getLogger(__name__)

# We compile the DDG4 plugin on the fly if it does not exist using the AClick mechanism:


def compileAClick(dictionary, g4=True):
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
  package = imp.find_module('DDG4')
  dic = os.path.dirname(package[1]) + os.sep + dictionary
  gInterpreter.ProcessLine('.L ' + dic + '+')
  from ROOT import dd4hep as module
  return module


def loaddd4hep():
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


OutputLevel = _Levels()
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
import_namespace_item('detail', 'eval')
#
#def run_interpreter(name):   detail.interp.run(name)
#def evaluator():     return eval.instance()
#def g4Evaluator():   return eval.g4instance()


def import_detail():
  import_namespace_item('detail', 'DD4hepUI')


def import_geometry():
  import_namespace_item('core', 'setPrintLevel')
  import_namespace_item('core', 'setPrintFormat')
  import_namespace_item('core', 'printLevel')
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
  import_namespace_item('core', 'Region')

  # // Readout.h
  import_namespace_item('core', 'Readout')

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
#
#  Import units from TGeo.
#  Calling import_units makes all the units local to the dd4hep module.
#
try:
  # from ROOT import TGeoUnit as TGeoUnits
  def import_units(ns=None):
    def import_unit(ns, nam):
      setattr(ns, nam, getattr(core, nam))
    items = [
        # Length
        'nanometer', 'micrometer', 'millimeter', 'centimeter', 'meter', 'kilometer', 'parsec', 'angstrom', 'fermi',
        'nm', 'um', 'mm', 'cm', 'm', 'km', 'pc',
        # Area
        'millimeter2', 'centimeter2', 'meter2', 'kilometer2', 'barn', 'millibarn', 'microbarn', 'nanobarn', 'picobarn',
        'mm2', 'cm2', 'm2', 'km2',
        # Volume
        'millimeter3', 'centimeter3', 'meter3', 'kilometer3', 'mm3', 'cm3', 'm3', 'km3',
        # Angle
        'radian', 'milliradian', 'degree', 'steradian', 'rad', 'mrad', 'sr', 'deg',
        # Time & frequency
        'nanosecond', 'second', 'millisecond', 'microsecond', 'picosecond', 'hertz',
        'kilohertz', 'megahertz', 'ns', 's', 'ms',
        # Electric charge
        'eplus', 'e_SI', 'coulomb',
        # Energy
        'electronvolt', 'kiloelectronvolt', 'megaelectronvolt', 'gigaelectronvolt', 'teraelectronvolt',
        'petaelectronvolt', 'joule', 'eV', 'keV', 'MeV', 'GeV', 'TeV', 'PeV',
        # Mass
        'milligram', 'gram', 'kilogram', 'mg', 'g', 'kg',
        # Power, Force, Pressure
        'watt', 'newton', 'hep_pascal', 'bar', 'atmosphere',
        # Electrical current, potential, resistance
        'nanoampere', 'microampere', 'milliampere', 'ampere', 'volt', 'kilovolt', 'megavolt', 'ohm',
        # Electric capacitance
        'picofarad', 'nanofarad', 'microfarad', 'millifarad', 'farad',
        # Magnetic flux, field, Inductance, Temperature, substance
        'weber', 'tesla', 'gauss', 'kilogauss', 'henry', 'kelvin', 'mole',
        # Activity, dose intensity
        'becquerel', 'curie', 'microgray', 'milligray', 'gray', 'kilogray',
        # Luminous intensity, flux, Illuminance
        'candela', 'lumen', 'lux',
        # Misc
        'perCent', 'perThousand', 'perMillion', 'pi', 'twopi', 'halfpi', 'pi2',
        'Avogadro', 'c_light', 'c_squared', 'h_Planck', 'hbar_Planck', 'hbarc', 'hbarc_squared', 'electron_charge',
        'e_squared', 'electron_mass_c2', 'proton_mass_c2', 'neutron_mass_c2', 'amu_c2', 'amu', 'mu0', 'epsilon0',
        #
        'elm_coupling', 'fine_structure_const', 'classic_electr_radius', 'electron_Compton_length', 'Bohr_radius',
        'alpha_rcl2', 'twopi_mc2_rcl2', 'k_Boltzmann', 'STP_Temperature', 'STP_Pressure',
        'kGasThreshold', 'universe_mean_density'
        ]
    if ns is None:
      ns = name_space
    logger.debug('Importing units into namespace ' + str(ns.__name__))
    for u in items:
      if u[0] != '_':
        import_unit(ns, u)
    return len(items)
except Exception as e:
  logger.warning('No units can be imported. %s' % str(e))

  def import_units(ns=None):
    return 0

import_units(ns=units)
