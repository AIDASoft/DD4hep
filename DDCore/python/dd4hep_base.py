#==========================================================================
#  AIDA Detector description implementation 
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
import logging

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
#
# We compile the DDG4 plugin on the fly if it does not exist using the AClick mechanism:
def compileAClick(dictionary,g4=True):
  from ROOT import gInterpreter, gSystem
  import sys, imp
  import os.path
  dd4hep = os.environ['DD4hepINSTALL']
  inc    = ' -I'+os.environ['ROOTSYS']+'/include -I'+dd4hep+'/include '
  lib    = ' -L'+dd4hep+'/lib -lDDCore -lDDG4 -lDDSegmentation '
  if g4:
    geant4 = os.environ['G4INSTALL']
    inc    = inc + ' -I'+geant4+'/include/Geant4 -Wno-shadow -g -O0 '
    lib    = lib + ' -L'+geant4+'/lib  -L'+geant4+'/lib64 -lG4event -lG4tracking -lG4particles '

  gSystem.AddIncludePath(inc)
  gSystem.AddLinkedLibs(lib)
  #####logging.info("Includes:   %s\n","Linked libs:%s",gSystem.GetIncludePath(),gSystem.GetLinkedLibs())
  logging.info('Loading AClick %s',dictionary)
  package = imp.find_module('DDG4')
  dic = os.path.dirname(package[1])+os.sep+dictionary
  ###logging.info(str(dic))
  gInterpreter.ProcessLine('.L '+dic+'+')
  #####gInterpreter.Load('DDG4Dict_C.so')
  from ROOT import dd4hep as module
  return module

def loaddd4hep():
  import os, sys
  # Add ROOT to the python path in case it is not yet there....
  sys.path.append(os.environ['ROOTSYS']+os.sep+'lib')
  import ROOT
  from ROOT import gSystem

  import platform
  if platform.system()=="Darwin":
    gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])

  result = gSystem.Load("libDDCore")
  if result < 0:
    raise Exception('dd4hep.py: Failed to load the dd4hep library libDDCore: '+gSystem.GetErrorStr())
  from ROOT import dd4hep as module
  return module

# We are nearly there ....
name_space = __import__(__name__)
def import_namespace_item(ns,nam):  
  scope = getattr(name_space,ns)
  attr = getattr(scope,nam)
  setattr(name_space,nam,attr)
  return attr

def import_root(nam):
  #logging.info('import ROOT class %s in namespace %s',nam,str(name_space))
  setattr(name_space,nam,getattr(ROOT,nam))

#---------------------------------------------------------------------------
#
try:
  dd4hep = loaddd4hep() 
  import ROOT
except Exception as X:
  import sys
  logging.info('+--%-100s--+',100*'-')
  logging.info('|  %-100s  |','Failed to load dd4hep base library:')
  logging.info('|  %-100s  |',str(X))
  logging.info('+--%-100s--+',100*'-')
  sys.exit(1)

class _Levels:
  def __init__(self):
    self.VERBOSE=1
    self.DEBUG=2
    self.INFO=3
    self.WARNING=4
    self.ERROR=5
    self.FATAL=6 
    self.ALWAYS=7

OutputLevel = _Levels()
#------------------------Generic STL stuff can be accessed using std:  -----
#
#-- e.g. Create an instance of std::vector<dd4hep::sim::Geant4Vertex*>:
#    >>> v=dd4hep.vector('dd4hep::sim::Geant4Vertex*')()
#                          
#---------------------------------------------------------------------------
import cppyy
std = cppyy.gbl.std
std_vector = std.vector
std_list   = std.list
std_map    = std.map
std_pair   = std.pair
#---------------------------------------------------------------------------
core   = dd4hep
cond   = dd4hep.cond
align  = dd4hep.align
detail = dd4hep.detail

import_root('XmlTools')
import_namespace_item('XmlTools','Evaluator')
#---------------------------------------------------------------------------
import_namespace_item('core','NamedObject')
import_namespace_item('core','run_interpreter')

def import_geometry():
  import_namespace_item('core','setPrintLevel')
  import_namespace_item('core','setPrintFormat')
  import_namespace_item('core','printLevel')
  import_namespace_item('core','Detector')
  import_namespace_item('core','evaluator')
  import_namespace_item('core','g4Evaluator')
  
  import_namespace_item('core','VolumeManager')
  import_namespace_item('core','OverlayedField')
  import_namespace_item('core','Ref_t')

  #// Objects.h
  import_namespace_item('core','Author')
  import_namespace_item('core','Header')
  import_namespace_item('core','Constant')
  import_namespace_item('core','Atom')
  import_namespace_item('core','Material')
  import_namespace_item('core','VisAttr')
  import_namespace_item('core','Limit')
  import_namespace_item('core','LimitSet')
  import_namespace_item('core','Region')

  #// Readout.h
  import_namespace_item('core','Readout')

  #// Alignments.h
  import_namespace_item('core','Alignment')
  import_namespace_item('core','AlignmentCondition')

  #// Conditions.h
  import_namespace_item('core','Condition')
  import_namespace_item('core','ConditionKey')

  #// DetElement.h
  import_namespace_item('core','World')
  import_namespace_item('core','DetElement')
  import_namespace_item('core','SensitiveDetector')

  #// Volume.h
  import_namespace_item('core','Volume')
  import_namespace_item('core','PlacedVolume')

  #// Shapes.h
  import_namespace_item('core','Solid')
  import_namespace_item('core','Box')
  import_namespace_item('core','HalfSpace')
  import_namespace_item('core','Polycone')
  import_namespace_item('core','ConeSegment')
  import_namespace_item('core','Tube')
  import_namespace_item('core','CutTube')
  import_namespace_item('core','TruncatedTube')
  import_namespace_item('core','EllipticalTube')
  import_namespace_item('core','Cone')
  import_namespace_item('core','Trap')
  import_namespace_item('core','PseudoTrap')
  import_namespace_item('core','Trapezoid')
  import_namespace_item('core','Torus')
  import_namespace_item('core','Sphere')
  import_namespace_item('core','Paraboloid')
  import_namespace_item('core','Hyperboloid')
  import_namespace_item('core','PolyhedraRegular')
  import_namespace_item('core','Polyhedra')
  import_namespace_item('core','ExtrudedPolygon')
  import_namespace_item('core','EightPointSolid')
  import_namespace_item('core','BooleanSolid')
  import_namespace_item('core','SubtractionSolid')
  import_namespace_item('core','UnionSolid')
  import_namespace_item('core','IntersectionSolid')


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
#
#  Import units from TGeo.
#  Calling import_units makes all the units local to the dd4hep module.
#
try:
  from ROOT import TGeoUnit as TGeoUnits
  def import_units(ns=None):
    def import_unit(ns,nam):
      setattr(ns,nam,getattr(TGeoUnits,nam))
    items = dir(TGeoUnits)
    if ns is None:
      ns = name_space
    print 'Importing TGeoUnits into namespace '+str(ns)
    for u in items:
      if u[0] != '_':
        import_unit(ns, u)
    return len(items)    
except:
  print 'WARNING: No units from TGeoUnit can be imported. This is normal for ROOT < 6.12.0'
  TGeoUnits = {}
  def import_units(ns=None):
    return 0
