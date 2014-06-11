from DD4hep import *

def loadDDG4():
  from ROOT import gSystem
  result = gSystem.Load("libDD4hepG4")
  if 0 != result:
    raise Exception('DDG4.py: Failed to load the Geant4 library libDD4hepG4: '+gSystem.GetErrorStr())
  from ROOT import DD4hep as module
  return module

# We are nearly there ....
current = __import__(__name__)
def _import_class(ns,nam):  
  scope = getattr(current,ns)
  setattr(current,nam,getattr(scope,nam))

#---------------------------------------------------------------------------
#
try:
  DD4hep     = loadDDG4() 
except Exception,X:
  print '+--%-100s--+'%(100*'-',)
  print '|  %-100s  |'%('Failed to load DDG4 library:',)
  print '|  %-100s  |'%(str(X),)
  print '|  %-100s  |'%('Try to compile AClick on the fly.',)
  print '+--%-100s--+'%(100*'-',)
  DD4hep   = compileAClick(dictionary='DDG4Dict.C',g4=True)  
Core       = DD4hep
Sim        = DD4hep.Simulation
Simulation = DD4hep.Simulation

Kernel     = Sim.KernelHandle
Interface  = Sim.Geant4ActionCreation

def _registerGlobalAction(self,action):
  self.get().registerGlobalAction(Interface.toAction(action))
def _registerGlobalFilter(self,filter):
  self.get().registerGlobalFilter(Interface.toAction(filter))
#---------------------------------------------------------------------------
def _getKernelProperty(self, name):
  import exceptions
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
