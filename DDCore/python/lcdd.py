from math import cos, sin, pi, tan
from os import path, listdir
from functools import partial
import SystemOfUnits
import math
from ROOT import SetOwnership, DD4hep, TGeoMixture, TGeoMedium, gGeoManager, TNamed

LCDD       = DD4hep.Geometry.LCDD
Constant   = DD4hep.Geometry.Constant
Material   = DD4hep.Geometry.Material
VisAttr    = DD4hep.Geometry.VisAttr
Limit      = DD4hep.Geometry.Limit
DetElement = DD4hep.Geometry.DetElement
Box        = DD4hep.Geometry.Box
Tube       = DD4hep.Geometry.Tube
Trapezoid  = DD4hep.Geometry.Trapezoid
Volume     = DD4hep.Geometry.Volume
PlacedVolume = DD4hep.Geometry.PlacedVolume
Position   = DD4hep.Geometry.Position
Rotation   = DD4hep.Geometry.Rotation
Handle     = DD4hep.Geometry.Handle
_toDictionary = DD4hep.Geometry._toDictionary

import xml.etree.ElementTree as xml
unique_mat_id = 0x7FFEFEED

current_xmlfile = None

constants = {}
constants.update(SystemOfUnits.__dict__)
constants.update(math.__dict__)
drivers = {}
drivers.update(math.__dict__)
drivers.update(DD4hep.Geometry.__dict__)


#---Enhancing the Element class with dedicated accessors--------------------------
def _getInt(self,attrib): return int(eval(self.get(attrib).replace('(int)',''),constants))
def _getFloat(self,*attrib):
  sval = self.get(attrib[0], None)
  if not sval and len(attrib) > 1: return attrib[1]
  else: return float(eval(sval.replace('(int)',''), constants))
def _getBool(self,attrib): return bool(self.get(attrib))
xml._ElementInterface.getI = _getInt
xml._ElementInterface.getF = _getFloat
xml._ElementInterface.getB = _getBool

xml._ElementInterface.name = property(lambda self: self.get('name'))
xml._ElementInterface.type = property(lambda self: self.get('type'))
xml._ElementInterface.vis  = property(lambda self: self.get('vis'))
xml._ElementInterface.material = property(lambda self: self.get('material'))
xml._ElementInterface.module = property(lambda self: self.get('module'))
xml._ElementInterface.id   = property(lambda self: self.getI('id'))
xml._ElementInterface.x1   = property(lambda self: self.getF('x1'))
xml._ElementInterface.x2   = property(lambda self: self.getF('x2'))
xml._ElementInterface.x    = property(lambda self: self.getF('x'))
xml._ElementInterface.y    = property(lambda self: self.getF('y'))
xml._ElementInterface.z    = property(lambda self: self.getF('z'))
xml._ElementInterface.zstart = property(lambda self: self.getF('zstart'))
xml._ElementInterface.phi0 = property(lambda self: self.getF('phi0'))
xml._ElementInterface.r    = property(lambda self: self.getF('r'))
xml._ElementInterface.dz   = property(lambda self: self.getF('dz'))
xml._ElementInterface.thickness = property(lambda self: self.getF('thickness'))
xml._ElementInterface.length = property(lambda self: self.getF('length'))
xml._ElementInterface.width = property(lambda self: self.getF('width'))
xml._ElementInterface.inner_r = property(lambda self: self.getF('inner_r'))
xml._ElementInterface.outer_r = property(lambda self: self.getF('outer_r'))
xml._ElementInterface.z_length = property(lambda self: self.getF('z_length'))



#---------------------------------------------------------------------------------
def load_drivers(*args):
  if not args:
    args = [path.join(path.dirname(__file__),'drivers')]
  for arg in args:
    if path.exists(arg):
      if path.isfile(arg):
        print "Loading driver file ... %s" % arg
        execfile(arg, drivers)
      elif path.isdir(arg):
        for f in listdir(arg) : 
          if path.splitext(f)[1] == '.py':
            print "Loading driver file ... %s" % path.join(arg,f)
            execfile(path.join(arg,f), drivers)
      else: raise "Path '%s' is not a directory or file" % arg 
    else: raise "Path '%s' does not exists" % arg


#---------------------------------------------------------------------------------
def process_xmlfile(lcdd, file):
  global current_xmlfile
  file = file.replace('file:','')
  root = xml.parse(file).getroot()
  last_xmlfile, current_xmlfile = current_xmlfile, file
  for e in root :
    if e.tag == 'detectors' : 
      lcdd.init() # call init before processing 'detectors' (need world volume)
    procs = globals().get('process_%s'% e.tag, None)
    if not procs : 
      procs = drivers.get('process_%s'% e.tag, None)
    if procs : 
      apply(procs,(lcdd, e))
    else : print 'XML tag %s not processed!!! No function found.' % e.tag
  current_xmlfile = last_xmlfile

#--------------------------------------------------------------------------------
def fromCompact(xmlfile):
  print 'Converting Compact file: ', xmlfile
  lcdd = LCDD.getInstance()
  lcdd.create()
  process_xmlfile(lcdd, xmlfile)
  return lcdd

#---------------------------------------------------------------------------------
def process_includes(lcdd, elem):
  for c in elem.findall('gdmlFile'):
    print 'Adding Gdml file ...', c.get('ref')
    fname = c.get('ref').replace('file:','')
    if not path.isabs(fname): fname = path.join(path.dirname(current_xmlfile),fname)
    process_xmlfile(lcdd, fname)
  for c in elem.findall('pyBuilder'):
    print 'Adding PyBuilder ...', c.get('ref')
    fname = c.get('ref')
    if not path.isabs(fname): fname = path.join(path.dirname(current_xmlfile),fname)
    load_drivers(fname)

#---------------------------------------------------------------------------------
def process_define(lcdd, elem):
  for c in elem.findall('constant'):
    print 'Adding constant ...', c.get('name')
    lcdd.addConstant(Constant(lcdd, c.get('name'),c.get('value')))
    _toDictionary(c.get('name'),c.get('value')) #-- Make it known to the evaluator
    constants[c.get('name')] = c.getF('value')

#---------------------------------------------------------------------------------
def process_element(lcdd, elem):
  #print 'Adding element ...', elem.get('name')
  ename = elem.get('name')
  tab = gGeoManager.GetElementTable()
  element = tab.FindElement(ename)
  if not element:
    atom = elem.find('atom')
    tab.AddElement(atom.get('name'), atom.get('formula'), atom.getI('Z'), atom.getI('value'))

#---------------------------------------------------------------------------------
def process_materials(lcdd, elem):
  for m in elem.findall('material'):
    process_material(lcdd, m)

#---------------------------------------------------------------------------------
def process_material(lcdd, m):
  #print 'Adding material ...', m.get('name')
  matname = m.get('name')
  density = m.find('D')
  composites = m.findall('fraction')
  table = gGeoManager.GetElementTable()
  mat = gGeoManager.GetMaterial(matname)
  if not mat:
    mat = TGeoMixture(matname, len(composites), density.getF('value'))
    SetOwnership( mat, False )
  elts = [mat.GetElement(i).GetName() for i in range(mat.GetNelements())]
  for c in composites:
    nam = c.get('ref')
    if nam not in elts:
      fraction = c.getF('n')
      if table.FindElement(nam):
        mat.AddElement(table.FindElement(nam), fraction)
      elif gGeoManager.GetMaterial(nam):
        mat.AddElement(gGeoManager.GetMaterial(nam), fraction)
      else:
        raise 'Something going very wrong. Undefined material:' + nam
  medium = gGeoManager.GetMedium(matname)
  if not medium:
    global unique_mat_id
    unique_mat_id = unique_mat_id - 1
    medium = TGeoMedium(matname, unique_mat_id, mat)
    SetOwnership(medium, False)
    medium.SetTitle('material')
    medium.SetUniqueID(unique_mat_id)
  lcdd.addMaterial(Handle(medium))


#----------------------------------------------------------------------------------
def process_display(lcdd, elem):
  for v in elem.findall('vis'):
    print 'Adding vis ...', v.name
    visattr = VisAttr(lcdd,v.name)
    r =  'r' in v.keys() and v.getF('r') or 1.0
    g =  'g' in v.keys() and v.getF('g') or 1.0
    b =  'b' in v.keys() and v.getF('b') or 1.0
    visattr.setColor(r,g,b)    
    if 'showDaughters' in v.keys() : visattr.setShowDaughters(v.getB('showDaughters'))
    if 'visible' in v.keys() : visattr.setVisible(v.getB('visible'))
    if 'alpha' in v.keys() : visattr.setAlpha(v.getF('alpha'))
    if 'lineStyle' in v.keys() :
      ls = v.get('lineStyle')
      if ls == 'unbroken' : visattr.setLineStyle(VisAttr.SOLID)
      if ls == 'broken' : visattr.setLineStyle(VisAttr.DASHED)
    else:
      visattr.setLineStyle(VisAttr.SOLID)
    if 'drawingStyle' in v.keys() :
      ds = v.get('drawingStyle')
      if ds == 'wireframe' : visattr.setDrawingStyle(VisAttr.WIREFRAME)
    lcdd.addVisAttribute(visattr)

def process_limits(lcdd, elem):
  # <limit name="step_length_max" particles="*" value="5.0" unit="mm" />    
  for l in elem.findall('limit'):
    limit = Limit(lcdd.document(), l.get('name'))
    limit.setParticles(l.get('particles'))
    limit.setValue(l.getF('value'))
    limit.setUnit(l.get('unit'))
    lcdd.addLimit(limit)

#-----------------------------------------------------------------------------------
def process_detectors(lcdd, elem):
  for d in elem.findall('detector'):
    procs = drivers.get('detector_%s'% d.get('type'), None)
    if procs : 
      detector = apply(procs,(lcdd, d))
      print "Adding detector ", detector
      lcdd.addDetector(detector)
    else : 
      print 'Detector type %s not found' % d.get('type')

