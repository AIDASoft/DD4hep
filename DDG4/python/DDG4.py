import os, sys, imp, exceptions
# Add ROOT to the python path in case it is not yet there....
sys.path.append(os.environ['ROOTSYS']+os.sep+'lib')
import ROOT
#---------------------------------------------------------------------------
# We compile the DDG4 plugin on the fly if it does not exist using the AClick mechanism:
def compileAClick(dictionary,g4=True):
  from ROOT import gInterpreter, gSystem
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

# We are nearly there ....
current = __import__(__name__)
def _import_class(ns,nam):  
  scope = getattr(current,ns)
  setattr(current,nam,getattr(scope,nam))

#---------------------------------------------------------------------------
DD4hep     = compileAClick(dictionary='DDG4Dict.C',g4=True) 
Sim        = DD4hep.Simulation
Simulation = DD4hep.Simulation

Kernel     = Sim.KernelHandle
Interface  = Sim.Geant4ActionCreation

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

def _registerGlobalAction(self,action):
  self.get().registerGlobalAction(Interface.toAction(action))
def _registerGlobalFilter(self,filter):
  self.get().registerGlobalFilter(Interface.toAction(filter))
#---------------------------------------------------------------------------
def _getKernelProperty(self, name):
  #print '_getKernelProperty:',str(type(self)),name
  ret = Interface.getPropertyKernel(self.get(),name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.get(),name):
    return getattr(self.get(),name)
  elif hasattr(self,name):
    return getattr(self,name)
  msg = 'Geant4Kernel::GetProperty [Unhandled]: Cannot access Kernel.'+name
  raise exceptions.KeyError(msg)

#---------------------------------------------------------------------------
def _setKernelProperty(self, name, value):
  #print '_setKernelProperty:',name,value
  if Interface.setPropertyKernel(self.get(),name,str(value)):
    return
  msg = 'Geant4Kernel::SetProperty [Unhandled]: Cannot set Kernel.'+name+' = '+str(value)
  raise exceptions.KeyError(msg)

Kernel.registerGlobalAction = _registerGlobalAction
Kernel.registerGlobalFilter = _registerGlobalFilter
Kernel.__getattr__ = _getKernelProperty
Kernel.__setattr__ = _setKernelProperty


ActionHandle                = Sim.ActionHandle
#---------------------------------------------------------------------------
def SensitiveAction(kernel,nam,det): return Interface.createSensitive(kernel,nam,det)
#---------------------------------------------------------------------------
def Action(kernel,nam):              return Interface.createAction(kernel,nam)
#---------------------------------------------------------------------------
def Filter(kernel,nam):              return Interface.createFilter(kernel,nam)
#---------------------------------------------------------------------------
def RunAction(kernel,nam):           return Interface.createRunAction(kernel,nam)
#---------------------------------------------------------------------------
def EventAction(kernel,nam):         return Interface.createEventAction(kernel,nam)
#---------------------------------------------------------------------------
def GeneratorAction(kernel,nam):     return Interface.createGeneratorAction(kernel,nam)
#---------------------------------------------------------------------------
def TrackingAction(kernel,nam):      return Interface.createTrackingAction(kernel,nam)
#---------------------------------------------------------------------------
def SteppingAction(kernel,nam):      return Interface.createSteppingAction(kernel,nam)
#---------------------------------------------------------------------------
def StackingAction(kernel,nam):      return Interface.createStackingAction(kernel,nam)
#---------------------------------------------------------------------------
def PhysicsList(kernel,nam):         return Interface.createPhysicsList(kernel,nam)
#---------------------------------------------------------------------------
def SensitiveSequence(kernel, nam):  return Interface.createSensDetSequence(kernel,nam)
#---------------------------------------------------------------------------
def _adopt(self,action):  self.adopt(action.get())
def _setup(obj):
    _import_class('Sim',obj)
    setattr(getattr(current,obj),'add',_adopt)
#---------------------------------------------------------------------------
_setup('Geant4RunActionSequence')
_setup('Geant4EventActionSequence')
_setup('Geant4GeneratorActionSequence')
_setup('Geant4TrackingActionSequence')
_setup('Geant4SteppingActionSequence')
_setup('Geant4StackingActionSequence')
_setup('Geant4PhysicsListActionSequence')
_setup('Geant4SensDetActionSequence')
_setup('Geant4Sensitive')
_import_class('Sim','Geant4Filter')
_import_class('Sim','Geant4RunAction')

#---------------------------------------------------------------------------
def _get(self, name):
  #print '_get:',str(type(self)),name
  a = Interface.toAction(self)
  ret = Interface.getProperty(a,name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.action,name):
    return getattr(self.action,name)
  elif hasattr(self,name):
    return getattr(self,name)
  msg = 'Geant4Action::GetProperty [Unhandled]: Cannot access '+a.name()+'.'+name
  raise exceptions.KeyError(msg)

def _set(self, name, value):
  #print '_set:',name,value
  a = Interface.toAction(self)
  if Interface.setProperty(a,name,str(value)):
    return
  msg = 'Geant4Action::SetProperty [Unhandled]: Cannot set '+a.name()+'.'+name+' = '+str(value)
  raise exceptions.KeyError(msg)

def _props(obj):
  _import_class('Sim',obj)
  cl = getattr(current,obj)
  cl.__getattr__ = _get
  cl.__setattr__ = _set

_props('FilterHandle')
_props('ActionHandle')
_props('RunActionHandle')
_props('EventActionHandle')
_props('GeneratorActionHandle')
_props('PhysicsListHandle')
_props('TrackingActionHandle')
_props('SteppingActionHandle')
_props('StackingActionHandle')
_props('SensitiveHandle')

_props('GeneratorActionSequenceHandle')
_props('RunActionSequenceHandle')
_props('EventActionSequenceHandle')
_props('TrackingActionSequenceHandle')
_props('SteppingActionSequenceHandle')
_props('StackingActionSequenceHandle')
_props('PhysicsListActionSequenceHandle')
_props('SensDetActionSequenceHandle')

_props('Geant4PhysicsListActionSequence')
#---------------------------------------------------------------------------
Geo        = DD4hep.Geometry
Geometry   = DD4hep.Geometry

_import_class('Geo','LCDD')
_import_class('Geo','VolumeManager')
_import_class('Geo','OverlayedField')

#// Objects.h
_import_class('Geo','Author')
_import_class('Geo','Header')
_import_class('Geo','Constant')
_import_class('Geo','Atom')
_import_class('Geo','Material')
_import_class('Geo','VisAttr')
_import_class('Geo','AlignmentEntry')
_import_class('Geo','Limit')
_import_class('Geo','LimitSet')
_import_class('Geo','Region')

#// Readout.h
_import_class('Geo','Readout')
_import_class('Geo','Alignment')
_import_class('Geo','Conditions')

#// DetElement.h
_import_class('Geo','DetElement')
_import_class('Geo','SensitiveDetector')

#// Volume.h
_import_class('Geo','Volume')
_import_class('Geo','PlacedVolume')

#// Shapes.h
_import_class('Geo','Polycone')
_import_class('Geo','ConeSegment')
_import_class('Geo','Box')
_import_class('Geo','Torus')
_import_class('Geo','Cone')
_import_class('Geo','Tube')
_import_class('Geo','Trap')
_import_class('Geo','Trapezoid')
_import_class('Geo','Sphere')
_import_class('Geo','Paraboloid')
_import_class('Geo','PolyhedraRegular')
_import_class('Geo','BooleanSolid')
_import_class('Geo','SubtractionSolid')
_import_class('Geo','UnionSolid')
_import_class('Geo','IntersectionSolid')

