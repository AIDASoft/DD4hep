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
AlignmentEntry = DD4hep.Geometry.AlignmentEntry
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
def _getBool(self,attrib): 
  return self.get(attrib, '').lower() in ('true', 'yes', 'on') 
xml._ElementInterface.getI = _getInt
xml._ElementInterface.getF = _getFloat
xml._ElementInterface.getB = _getBool


xml._ElementInterface.name = property(lambda self: self.get('name'))
xml._ElementInterface.type = property(lambda self: self.get('type'))
xml._ElementInterface.vis  = property(lambda self: self.get('vis'))
xml._ElementInterface.ref  = property(lambda self: self.get('ref'))
xml._ElementInterface.value  = property(lambda self: self.getF('value'))
xml._ElementInterface.material = property(lambda self: self.get('material'))
xml._ElementInterface.module = property(lambda self: self.get('module'))
xml._ElementInterface.id   = property(lambda self: self.getI('id'))
xml._ElementInterface.number = property(lambda self: self.getI('number'))
xml._ElementInterface.x1   = property(lambda self: self.getF('x1'))
xml._ElementInterface.x2   = property(lambda self: self.getF('x2'))
xml._ElementInterface.x    = property(lambda self: self.getF('x'))
xml._ElementInterface.y    = property(lambda self: self.getF('y'))
xml._ElementInterface.z    = property(lambda self: self.getF('z'))
xml._ElementInterface.zstart = property(lambda self: self.getF('zstart'))
xml._ElementInterface.offset = property(lambda self: self.getF('offset'))
xml._ElementInterface.radius = property(lambda self: self.getF('radius'))
xml._ElementInterface.zhalf = property(lambda self: self.getF('zhalf'))
xml._ElementInterface.phi0 = property(lambda self: self.getF('phi0'))
xml._ElementInterface.r    = property(lambda self: self.getF('r'))
xml._ElementInterface.dz   = property(lambda self: self.getF('dz'))
xml._ElementInterface.thickness = property(lambda self: self.getF('thickness'))
xml._ElementInterface.length = property(lambda self: self.getF('length'))
xml._ElementInterface.width = property(lambda self: self.getF('width'))
xml._ElementInterface.inner_r = property(lambda self: self.getF('inner_r'))
xml._ElementInterface.outer_r = property(lambda self: self.getF('outer_r'))
xml._ElementInterface.z_length = property(lambda self: self.getF('z_length'))
xml._ElementInterface.rmin = property(lambda self: self.getF('rmin'))
xml._ElementInterface.rmax = property(lambda self: self.getF('rmax'))


def getRotation(rot):
  if rot is not None : return Rotation(rot.getF('x',0.0),rot.getF('y',0.0), rot.getF('z',0.0))
  else   : return Rotation()

def getPosition(pos):
  if pos is not None : return Position(pos.getF('x',0.0),pos.getF('y',0.0), pos.getF('z',0.0))
  else   : return Position()

drivers['getRotation'] = getRotation
drivers['getPosition'] = getPosition


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
  for c in elem.findall('alignment'):
    print 'Adding Alignment file ...', c.get('ref')
    fname = c.get('ref').replace('file:','')
    if not path.isabs(fname): fname = path.join(path.dirname(current_xmlfile),fname)
    process_xmlfile(lcdd, fname)

#---------------------------------------------------------------------------------
def process_info(lcdd, elem):
  pass

#---------------------------------------------------------------------------------
def process_define(lcdd, elem):
  for c in elem.findall('constant'):
    #print 'Adding constant ...', c.get('name')
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
# <material formula="Ac" name="Actinium" state="solid" >
#  <RL type="X0" unit="cm" value="0.601558" />
#  <NIL type="lambda" unit="cm" value="21.2048" />
#  <D type="density" unit="g/cm3" value="10.07" />
#  <composite n="1" ref="Ac" />
#</material>
#<material name="G10">
#  <D type="density" value="1.7" unit="g/cm3"/>
#  <fraction n="0.08" ref="Cl"/>
#  <fraction n="0.773" ref="Quartz"/>
#  <fraction n="0.147" ref="Epoxy"/>
#</material>


def process_material(lcdd, m):
  #print 'Adding material ...', m.get('name')
  density = m.find('D')
  radlen  = m.find('RL')
  intlen  = m.find('NIL')
  composites = m.findall('fraction') or m.findall('composite')
  table = gGeoManager.GetElementTable()
  mat = gGeoManager.GetMaterial(m.name)
  if not mat:
    mat = TGeoMixture(m.name, len(composites), eval(density.get('value')+'*'+density.get('unit')+'/(g/cm3)',constants))
    SetOwnership( mat, False )
  rl = (radlen is not None) and eval(radlen.get('value')+'*'+radlen.get('unit'),constants) or 0.0
  il = (intlen is not None) and eval(intlen.get('value')+'*'+intlen.get('unit'),constants) or 0.0
  #mat.SetRadLen(-rl, -il)
  elts = [mat.GetElement(i).GetName() for i in range(mat.GetNelements())]
  for c in composites:
    nam = c.ref
    if nam not in elts:
      if c.tag == 'composite' : fraction = c.getI('n')
      elif c.tag == 'fraction' : fraction = c.getF('n')
      if table.FindElement(nam):
        mat.AddElement(table.FindElement(nam), fraction)
      elif gGeoManager.GetMaterial(nam):
        mat.AddElement(gGeoManager.GetMaterial(nam), fraction)
      else:
        raise 'Something going very wrong. Undefined material:' + nam
  medium = gGeoManager.GetMedium(m.name)
  if not medium:
    global unique_mat_id
    unique_mat_id = unique_mat_id - 1
    medium = TGeoMedium(m.name, unique_mat_id, mat)
    SetOwnership(medium, False)
    medium.SetTitle('material')
    medium.SetUniqueID(unique_mat_id)
  lcdd.addMaterial(Handle(medium))


#----------------------------------------------------------------------------------
def process_display(lcdd, elem):
  for v in elem.findall('vis'):
    #print 'Adding vis ...', v.name
    visattr = VisAttr(lcdd, v.name)
    r,g,b = 1.,1.,1.
    if 'r' in v.keys() : r = v.getF('r')    
    if 'g' in v.keys() : g = v.getF('g')
    if 'b' in v.keys() : b = v.getF('b')
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
    #print visattr.toString()
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
      lcdd.addDetector(detector)
    else : 
      print 'Detector type %s not found' % d.get('type')

#-----------------------------------------------------------------------------------
def process_alignments(lcdd, elem):
  for a in elem.findall('alignment'):
    process_alignment(lcdd, a)

#-----------------------------------------------------------------------------------
def process_alignment(lcdd, elem):
  alignment = AlignmentEntry(lcdd, elem.name)
  pos = getPosition(elem.find('position'))
  rot = getRotation(elem.find('rotation'))
  print pos.isNull(), rot.isNull()
  alignment.align(pos,rot)
  return alignment



