#---------------------------------------------------------------------------
# We compile the DDG4 plugin on the fly if it does not exist using the AClick mechanism:
def compileAClick(dictionary,g4=True):
  from ROOT import gInterpreter, gSystem
  import sys, imp, exceptions
  import os.path
  dd4hep = os.environ['DD4hepINSTALL']
  inc    = ' -I'+os.environ['ROOTSYS']+'/include -I'+dd4hep+'/include '
  lib    = ' -L'+dd4hep+'/lib -lDD4hepCore -lDD4hepG4 -lDDSegmentation '
  if g4:
    geant4 = os.environ['G4INSTALL']
    inc    = inc + ' -I'+geant4+'/include/Geant4 -Wno-shadow -g -O0 '
    lib    = lib + ' -L'+geant4+'/lib  -L'+geant4+'/lib64 -lG4event -lG4tracking -lG4particles '

  gSystem.AddIncludePath(inc)
  gSystem.AddLinkedLibs(lib)
  #####print "Includes:   ",gSystem.GetIncludePath(),"\n","Linked libs:",gSystem.GetLinkedLibs()
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
  result = gSystem.Load("libDD4hepCore")
  if 0 != result:
    raise Exception('DDG4.py: Failed to load the Geant4 library libDD4hepCore: '+gSystem.GetErrorStr())
  from ROOT import DD4hep as module
  return module

# We are nearly there ....
name_space = __import__(__name__)
def import_class(ns,nam):  
  scope = getattr(name_space,ns)
  setattr(name_space,nam,getattr(scope,nam))

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
  print '|  %-100s  |'%('Failed to load DDG4 library:',)
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
#---------------------------------------------------------------------------
import_class('Core','NamedObject')
import_class('Core','run_interpreter')

def import_geometry():
  import_class('Geo','LCDD')
  import_class('Geo','VolumeManager')
  import_class('Geo','OverlayedField')

  #// Objects.h
  import_class('Geo','Author')
  import_class('Geo','Header')
  import_class('Geo','Constant')
  import_class('Geo','Atom')
  import_class('Geo','Material')
  import_class('Geo','VisAttr')
  import_class('Geo','AlignmentEntry')
  import_class('Geo','Limit')
  import_class('Geo','LimitSet')
  import_class('Geo','Region')

  #// Readout.h
  import_class('Geo','Readout')
  import_class('Geo','Alignment')
  import_class('Geo','Conditions')

  #// DetElement.h
  import_class('Geo','DetElement')
  import_class('Geo','SensitiveDetector')

  #// Volume.h
  import_class('Geo','Volume')
  import_class('Geo','PlacedVolume')

  #// Shapes.h
  import_class('Geo','Polycone')
  import_class('Geo','ConeSegment')
  import_class('Geo','Box')
  import_class('Geo','Torus')
  import_class('Geo','Cone')
  import_class('Geo','Tube')
  import_class('Geo','Trap')
  import_class('Geo','Trapezoid')
  import_class('Geo','Sphere')
  import_class('Geo','Paraboloid')
  import_class('Geo','PolyhedraRegular')
  import_class('Geo','BooleanSolid')
  import_class('Geo','SubtractionSolid')
  import_class('Geo','UnionSolid')
  import_class('Geo','IntersectionSolid')


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
