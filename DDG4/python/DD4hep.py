#==========================================================================
#  AIDA Detector description implementation for LCD
#--------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
#==========================================================================
#
# We compile the DDG4 plugin on the fly if it does not exist using the AClick mechanism:
def compileAClick(dictionary,g4=True):
  from ROOT import gInterpreter, gSystem
  import sys, imp, exceptions
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
  #####print "Includes:   ",gSystem.GetIncludePath(),"\n","Linked libs:",gSystem.GetLinkedLibs()
  print 'Loading AClick ',dictionary
  package = imp.find_module('DDG4')
  dic = os.path.dirname(package[1])+os.sep+dictionary
  ###print dic
  gInterpreter.ProcessLine('.L '+dic+'+')
  #####gInterpreter.Load('DDG4Dict_C.so')
  from ROOT import DD4hep as module
  return module

def loadDD4hep():
  import os, sys
  # Add ROOT to the python path in case it is not yet there....
  sys.path.append(os.environ['ROOTSYS']+os.sep+'lib')
  import ROOT
  from ROOT import gSystem

  import platform
  if platform.system()=="Darwin":
    gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])

  result = gSystem.Load("libDDCore")
  if 0 != result:
    raise Exception('DD4hep.py: Failed to load the DD4hep library libDDCore: '+gSystem.GetErrorStr())
  from ROOT import DD4hep as module
  return module

# We are nearly there ....
name_space = __import__(__name__)
def import_namespace_item(ns,nam):  
  scope = getattr(name_space,ns)
  attr = getattr(scope,nam)
  setattr(name_space,nam,attr)
  return attr

def import_root(nam):
  #print 'import ROOT class ',nam,str(name_space)
  setattr(name_space,nam,getattr(ROOT,nam))

#---------------------------------------------------------------------------
#
try:
  DD4hep = loadDD4hep() 
  import ROOT
except Exception,X:
  import sys
  print '+--%-100s--+'%(100*'-',)
  print '|  %-100s  |'%('Failed to load DD4hep base library:',)
  print '|  %-100s  |'%(str(X),)
  print '+--%-100s--+'%(100*'-',)
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
#---------------------------------------------------------------------------
Core       = DD4hep
Geo        = DD4hep.Geometry
Geometry   = DD4hep.Geometry
Conditions = DD4hep.Conditions
Alignments = DD4hep.Alignments

import_root('XmlTools')
import_namespace_item('XmlTools','Evaluator')
#---------------------------------------------------------------------------
import_namespace_item('Core','NamedObject')
import_namespace_item('Core','run_interpreter')

def import_geometry():
  import_namespace_item('Core','setPrintLevel')
  import_namespace_item('Core','setPrintFormat')
  import_namespace_item('Core','printLevel')
  import_namespace_item('Geo','LCDD')
  import_namespace_item('Core','evaluator')
  import_namespace_item('Core','g4Evaluator')
  
  import_namespace_item('Geo','VolumeManager')
  import_namespace_item('Geo','OverlayedField')
  import_namespace_item('Geo','Ref_t')

  #// Objects.h
  import_namespace_item('Geo','Author')
  import_namespace_item('Geo','Header')
  import_namespace_item('Geo','Constant')
  import_namespace_item('Geo','Atom')
  import_namespace_item('Geo','Material')
  import_namespace_item('Geo','VisAttr')
  import_namespace_item('Geo','AlignmentEntry')
  import_namespace_item('Geo','Limit')
  import_namespace_item('Geo','LimitSet')
  import_namespace_item('Geo','Region')

  #// Readout.h
  import_namespace_item('Geo','Readout')
  import_namespace_item('Alignments','Alignment')
  import_namespace_item('Alignments','Container')
  import_namespace_item('Conditions','Condition')
  import_namespace_item('Conditions','Container')

  #// DetElement.h
  import_namespace_item('Geo','World')
  import_namespace_item('Geo','DetElement')
  import_namespace_item('Geo','SensitiveDetector')

  #// Volume.h
  import_namespace_item('Geo','Volume')
  import_namespace_item('Geo','PlacedVolume')

  #// Shapes.h
  import_namespace_item('Geo','Polycone')
  import_namespace_item('Geo','ConeSegment')
  import_namespace_item('Geo','Box')
  import_namespace_item('Geo','Torus')
  import_namespace_item('Geo','Cone')
  import_namespace_item('Geo','Tube')
  import_namespace_item('Geo','Trap')
  import_namespace_item('Geo','Trapezoid')
  import_namespace_item('Geo','Sphere')
  import_namespace_item('Geo','Paraboloid')
  import_namespace_item('Geo','PolyhedraRegular')
  import_namespace_item('Geo','BooleanSolid')
  import_namespace_item('Geo','SubtractionSolid')
  import_namespace_item('Geo','UnionSolid')
  import_namespace_item('Geo','IntersectionSolid')


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
