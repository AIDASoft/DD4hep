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
from __future__ import absolute_import

from math import cos, sin, pi, tan
from os import path, listdir
from functools import partial
import SystemOfUnits
import math
import logging
from ROOT import SetOwnership, dd4hep, TGeoMixture, TGeoMedium, gGeoManager, TNamed

logging.basicConfig(format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

Detector       = dd4hep.Geometry.Detector
Constant   = dd4hep.Geometry.Constant
Material   = dd4hep.Geometry.Material
VisAttr    = dd4hep.Geometry.VisAttr
AlignmentEntry = dd4hep.Geometry.AlignmentEntry
Limit      = dd4hep.Geometry.Limit
DetElement = dd4hep.Geometry.DetElement
Box        = dd4hep.Geometry.Box
Tube       = dd4hep.Geometry.Tube
Trapezoid  = dd4hep.Geometry.Trapezoid
Volume     = dd4hep.Geometry.Volume
PlacedVolume = dd4hep.Geometry.PlacedVolume
Position   = dd4hep.Geometry.Position
Rotation   = dd4hep.Geometry.Rotation
Handle     = dd4hep.Geometry.Handle
Readout    = dd4hep.Geometry.Readout
GridXYZ = dd4hep.Geometry.GridXYZ
GlobalGridXY = dd4hep.Geometry.GlobalGridXY
CartesianGridXY = dd4hep.Geometry.CartesianGridXY
NoSegmentation = dd4hep.Geometry.NoSegmentation
GridPhiEta = dd4hep.Geometry.GridPhiEta
GridRPhiEta = dd4hep.Geometry.GridRPhiEta
ProjectiveCylinder = dd4hep.Geometry.ProjectiveCylinder
NonProjectiveCylinder = dd4hep.Geometry.NonProjectiveCylinder
ProjectiveZPlane = dd4hep.Geometry.ProjectiveZPlane
IDDescriptor = dd4hep.Geometry.IDDescriptor

_toDictionary = dd4hep.Geometry._toDictionary

import xml.etree.ElementTree as xml
unique_mat_id = 0x7FFEFEED

current_xmlfile = None

constants = {}
constants.update(SystemOfUnits.__dict__)
constants.update(math.__dict__)
drivers = {}
drivers.update(math.__dict__)
drivers.update(dd4hep.Geometry.__dict__)


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
        logger.info("Loading driver file ... %s" % arg)
        execfile(arg, drivers)
      elif path.isdir(arg):
        for f in listdir(arg) : 
          if path.splitext(f)[1] == '.py':
            logger.info("Loading driver file ... %s" % path.join(arg,f))
            execfile(path.join(arg,f), drivers)
      else: raise "Path '%s' is not a directory or file" % arg 
    else: raise "Path '%s' does not exists" % arg


#---------------------------------------------------------------------------------
def process_xmlfile(description, file):
  global current_xmlfile
  file = file.replace('file:','')
  root = xml.parse(file).getroot()
  last_xmlfile, current_xmlfile = current_xmlfile, file
  tags = ('includes', 'define', 'materials', 'properties', 'limits', 'display',
          'readouts', 'detectors', 'alignments', 'fields', 'sensitive_detectors')
  if root.tag in tags :
    process_tag(description, root)
  else :
    for tag in tags:
      for e in root.findall(tag):
        process_tag(description, e)
  current_xmlfile = last_xmlfile

def process_tag(description, elem):
  if elem.tag == 'detectors' :
    description.init() # call init before processing 'detectors' (need world volume)
  procs = globals().get('process_%s'% elem.tag, None)
  if not procs :
    procs = drivers.get('process_%s'% elem.tag, None)
  if procs :
    apply(procs,(description, elem))
  else : logger.info('XML tag %s not processed!!! No function found.' % elem.tag)


#--------------------------------------------------------------------------------
def fromXML(xmlfile):
  logger.info('Converting Compact file: %s', xmlfile)
  description = Detector.getInstance()
  #description.create()
  process_xmlfile(description, xmlfile)
  return description

#---------------------------------------------------------------------------------
def process_includes(description, elem):
  for c in elem.findall('gdmlFile'):
    logger.info('Adding Gdml file ... %s', c.get('ref'))
    fname = c.get('ref').replace('file:','')
    if not path.isabs(fname): fname = path.join(path.dirname(current_xmlfile),fname)
    process_xmlfile(description, fname)
  for c in elem.findall('pyBuilder'):
    logger.info('Adding PyBuilder ... %s', c.get('ref'))
    fname = c.get('ref')
    if not path.isabs(fname): fname = path.join(path.dirname(current_xmlfile),fname)
    load_drivers(fname)
  for c in elem.findall('alignment'):
    logger.info('Adding Alignment file ... %s', c.get('ref'))
    fname = c.get('ref').replace('file:','')
    if not path.isabs(fname): fname = path.join(path.dirname(current_xmlfile),fname)
    process_xmlfile(description, fname)

#---------------------------------------------------------------------------------
def process_info(description, elem):
  pass

#---------------------------------------------------------------------------------
def process_define(description, elem):
  for c in elem.findall('constant'):
    description.addConstant(Constant(c.get('name'),c.get('value')))
    _toDictionary(c.get('name'),c.get('value')) #-- Make it known to the evaluator
    constants[c.get('name')] = c.getF('value')

#---------------------------------------------------------------------------------
def process_element(description, elem):
  ename = elem.get('name')
  tab = gGeoManager.GetElementTable()
  element = tab.FindElement(ename)
  if not element:
    atom = elem.find('atom')
    tab.AddElement(atom.get('name'), atom.get('formula'), atom.getI('Z'), atom.getI('value'))

#---------------------------------------------------------------------------------
def process_materials(description, elem):
  for m in elem.findall('material'):
    process_material(description, m)

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


def process_material(description, m):
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
  description.addMaterial(Handle(medium))


#----------------------------------------------------------------------------------
def process_display(description, elem):
  for v in elem.findall('vis'):
    visattr = VisAttr(v.name)
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
    description.addVisAttribute(visattr)

def process_limits(description, elem):
  # <limit name="step_length_max" particles="*" value="5.0" unit="mm" />    
  for l in elem.findall('limit'):
    limit = Limit(description.document(), l.get('name'))
    limit.setParticles(l.get('particles'))
    limit.setValue(l.getF('value'))
    limit.setUnit(l.get('unit'))
    description.addLimit(limit)

#-----------------------------------------------------------------------------------
def process_detectors(description, elem):
  for d in elem.findall('detector'):
    procs = drivers.get('detector_%s'% d.get('type'), None)
    if procs : 
      detector = apply(procs,(description, d))
      description.addDetector(detector)
    else : 
      logger.info('Detector type %s not found' % d.get('type'))

#-----------------------------------------------------------------------------------
def process_alignments(description, elem):
  for a in elem.findall('alignment'):
    process_alignment(description, a)

#-----------------------------------------------------------------------------------
def process_alignment(description, elem):
  alignment = AlignmentEntry(description, elem.name)
  pos = getPosition(elem.find('position'))
  rot = getRotation(elem.find('rotation'))
  logger.info("%s %s", pos.isNull(), rot.isNull())
  alignment.align(pos,rot)
  return alignment

#-----------------------------------------------------------------------------------
def process_readouts(description, elem):
  for a in elem.findall('readout'):
    process_readout(description, a)

#-----------------------------------------------------------------------------------
def process_readout(description, elem):
  readout = Readout(elem.name)
  seg = elem.find('segmentation')
  if seg is not None:
    procs = globals().get('create_%s'% seg.get('type'), None)
    if not procs :
      procs = drivers.get('create_%s'% seg.get('type'), None)
    if procs :
      segment = apply(procs,(description, seg))
      readout.setSegmentation(segment)
    else :
      logger.info('Segmentation type %s not found' % seg.get('type'))
  id = elem.find('id')
  if id is not None:
    idSpec = IDDescriptor(id.text)
    idSpec.SetName(elem.name)
    readout.setIDDescriptor(idSpec)
    description.addIDSpecification(idSpec)
  description.addReadout(readout)

#---Segmentations--------------------------------------------------------------------
def create_GridXYZ(description, elem) :
  obj = GridXYZ()
  if 'gridSizeX' in elem.keys() : obj.setGridSizeX(elem.getF('gridSizeX'))
  if 'gridSizeY' in elem.keys() : obj.setGridSizeY(elem.getF('gridSizeY'))
  if 'gridSizeZ' in elem.keys() : obj.setGridSizeZ(elem.getF('gridSizeZ'))
  return obj

def create_GlobalGridXY(description, elem) :
  obj = GlobalGridXY()
  if 'gridSizeX' in elem.keys() : obj.setGridSizeX(elem.getF('gridSizeX'))
  if 'gridSizeY' in elem.keys() : obj.setGridSizeY(elem.getF('gridSizeY'))
  return obj

def create_CartesianGridXY(description, elem) :
  obj = CartesianGridXY()
  if 'gridSizeX' in elem.keys() : obj.setGridSizeX(elem.getF('gridSizeX'))
  if 'gridSizeY' in elem.keys() : obj.setGridSizeY(elem.getF('gridSizeY'))
  return obj

def create_NoSegmentation(description, elem) :
  obj = NoSegmentation()
  return obj

def create_ProjectiveCylinder(description, elem) :
  obj = ProjectiveCylinder()
  if 'phiBins' in elem.keys() : obj.setPhiBins(elem.getI('phiBins'))
  if 'thetaBins' in elem.keys() : obj.setThetaBins(elem.getI('thetaBins'))
  return obj

def create_NonProjectiveCylinder(description, elem) :
  obj = NonProjectiveCylinder()
  if 'gridSizePhi' in elem.keys() : obj.setThetaBinSize(elem.getF('gridSizePhi'))
  if 'gridSizeZ' in elem.keys() : obj.setPhiBinSize(elem.getI('gridSizeZ'))
  return obj

def create_ProjectiveZPlane(description, elem) :
  obj = ProjectiveZPlaner()
  if 'phiBins' in elem.keys() : obj.setPhiBins(elem.getI('phiBins'))
  if 'thetaBins' in elem.keys() : obj.setThetaBins(elem.getI('thetaBins'))
  return obj

def create_GridPhiEta(description, elem) :
  obj = GridPhiEta()
  if 'phiBins' in elem.keys() : obj.setPhiBins(elem.getI('phiBins'))
  if 'gridSizeEta' in elem.keys() : obj.setGridSizeEta(elem.getI('gridSizeEta'))
  return obj

def create_GridRPhiEta(description, elem) :
  obj = GridRPhiEta()
  if 'phiBins' in elem.keys() : obj.setPhiBins(elem.getI('gridSizeR'))
  if 'gridSizeEta' in elem.keys() : obj.setGridSizeEta(elem.getI('gridSizeEta'))
  if 'gridSizeR' in elem.keys() : obj.setGridSizeR(elem.getI('gridSizeR'))
  return obj


