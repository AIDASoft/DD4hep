from math import cos, sin, pi, tan
from os import path, listdir
import SystemOfUnits
import math
from ROOT import SetOwnership, DetDesc, TGeoMixture, TGeoMedium

LCDD     = DetDesc.Geometry.LCDD
Constant = DetDesc.Geometry.Constant
Material = DetDesc.Geometry.Material
VisAttr  = DetDesc.Geometry.VisAttr
Limit    = DetDesc.Geometry.Limit
Subdetector = DetDesc.Geometry.Subdetector
Box = DetDesc.Geometry.Box
Tube = DetDesc.Geometry.Tube
Volume = DetDesc.Geometry.Volume
PhysVol = DetDesc.Geometry.PhysVol
Position = DetDesc.Geometry.Position
Rotation = DetDesc.Geometry.Rotation
Handle_t = DetDesc.Geometry.Handle_t
_toDictionary = DetDesc.Geometry._toDictionary

import xml.etree.ElementTree as xml
unique_mat_id = 0x7FFEFEED

current_xmlfile = None

constants = {}
constants.update(SystemOfUnits.__dict__)
drivers = {}
drivers.update(math.__dict__)
drivers.update(DetDesc.Geometry.__dict__)


#---Enhancing the Element class with dedicated accessors--------------------------
def _getInt(self,attrib):
  return int(eval(self.get(attrib),constants))
def _getFloat(self,attrib):
  return float(eval(self.get(attrib),constants))
def _getBool(self,attrib):
  return bool(self.get(attrib))

xml._ElementInterface.getI = _getInt
xml._ElementInterface.getF = _getFloat
xml._ElementInterface.getB = _getBool

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
    dname = path.dirname(current_xmlfile)
    process_xmlfile(lcdd, path.join(dname, c.get('ref')))
  for c in elem.findall('pyBuilder'):
    print 'Adding PyBuilder ...', c.get('ref')
    load_drivers(path.join(dname, c.get('ref')))

#---------------------------------------------------------------------------------
def process_define(lcdd, elem):
  for c in elem.findall('constant'):
    #print 'Adding constant ...', c.get('name')
    lcdd.addConstant(Constant(lcdd.document(), c.get('name'),c.get('value')))
    _toDictionary(c.get('name'),c.get('value')) #-- Make it known to the evaluator
    constants[c.get('name')] = c.getF('value')

#---------------------------------------------------------------------------------
def process_element(lcdd, elem):
  #print 'Adding element ...', elem.get('name')
  doc = lcdd.document()
  ename = elem.get('name')
  tab = doc.GetElementTable()
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
  doc     = lcdd.document()
  matname = m.get('name')
  density = m.find('D')
  composites = m.findall('fraction')
  table = doc.GetElementTable()
  mat = doc.GetMaterial(matname)
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
      elif doc.GetMaterial(nam):
        mat.AddElement(doc.GetMaterial(nam), fraction)
      else:
        raise 'Something going very wrong. Undefined material:' + nam
  medium = doc.GetMedium(matname)
  if not medium:
    global unique_mat_id
    unique_mat_id = unique_mat_id - 1
    medium = TGeoMedium(matname, unique_mat_id, mat)
    SetOwnership(medium, False)
    medium.SetTitle('material')
    medium.SetUniqueID(unique_mat_id)
  lcdd.addMaterial(Handle_t(medium))


#----------------------------------------------------------------------------------
def process_display(lcdd, elem):
  for v in elem.findall('vis'):
    #print 'Adding vis ...', v.get('name')
    visattr = VisAttr(lcdd.document(),v.get('name'))
    if 'showDaughters' in v.keys() : visattr.setShowDaughters(v.getB('showDaughters'))
    if 'visible' in v.keys() : visattr.setVisible(v.getB('visible'))
    if 'alpha' in v.keys() : visattr.setAlpha(v.getF('alpha'))
    if 'r' in v.keys() and 'g' in v.keys() and 'b' in v.keys() : visattr.setColor(v.getF('r'),v.getF('g'),v.getF('b'))
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

