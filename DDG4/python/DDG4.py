from DD4hep import *

def loadDDG4():
  from ROOT import gSystem
  result = gSystem.Load("libDD4hepG4Plugins")
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
LCDD       = Geo.LCDD

#---------------------------------------------------------------------------
def _constant(self,name):
  return self.constantAsString(name)

LCDD.globalVal = _constant
#---------------------------------------------------------------------------

"""
  Import the LCDD constants into the DDG4 namespace
"""
def importConstants(lcdd,namespace=None,debug=False):
  scope = current
  ns = current
  if namespace is not None and not hasattr(current,namespace):
    import imp
    m = imp.new_module('DDG4.'+namespace)
    setattr(current,namespace,m)
    ns = m
  evaluator = DD4hep.g4Evaluator()
  cnt = 0
  num = 0
  todo = {}
  for c in lcdd.constants(): 
    todo[c.first] = c.second.GetTitle().replace('(int)','')
  while len(todo) and cnt<100:
    cnt = cnt + 1
    if cnt == 100:
      print '%s %d out of %d %s "%s": [%s]\n+++ %s'\
          %('+++ FAILED to import',
            len(todo),len(todo)+num,
            'global values into namespace',
            ns.__name__,'Try to continue anyway',100*'=',)
      for k,v in todo.items():
        if not hasattr(ns,k):
          print '+++ FAILED to import: "'+k+'" = "'+str(v)+'"'
      print '+++ %s'%(100*'=',)

    for k,v in todo.items():
      if not hasattr(ns,k):
        val = evaluator.evaluate(v)
        status = evaluator.status()
        if status == 0:
          evaluator.setVariable(k,val)
          setattr(ns,k,val)
          if debug: print 'Imported global value: "'+k+'" = "'+str(val)+'" into namespace',ns.__name__
          del todo[k]
          num = num + 1
  if cnt<100:
    print '+++ Imported %d global values to namespace:%s'%(num,ns.__name__,)

#---------------------------------------------------------------------------  
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
  import exceptions
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
def _setup(obj):
  def _adopt(self,action):  self.__adopt(action.get())
  _import_class('Sim',obj)
  o = getattr(current,obj)
  setattr(o,'__adopt',getattr(o,'adopt'))
  setattr(o,'adopt',_adopt)
  setattr(o,'add',_adopt)

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
_setup('Geant4ParticleHandler')
_import_class('Sim','Geant4Filter')
_import_class('Sim','Geant4RunAction')
_import_class('Sim','Geant4UserParticleHandler')

#---------------------------------------------------------------------------
def _get(self, name):
  import exceptions, traceback
  #print '_get:',str(type(self)),name
  a = Interface.toAction(self)
  ret = Interface.getProperty(a,name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.action,name):
    return getattr(self.action,name)
  elif hasattr(a,name):
    return getattr(a,name)
  #elif hasattr(self,name):
  #  return getattr(self,name)
  #traceback.print_stack()
  msg = 'Geant4Action::GetProperty [Unhandled]: Cannot access property '+a.name()+'.'+name
  raise exceptions.KeyError(msg)

def _set(self, name, value):
  import exceptions
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
_props('Geant4ParticleHandler')
_props('Geant4UserParticleHandler')

_props('GeneratorActionSequenceHandle')
_props('RunActionSequenceHandle')
_props('EventActionSequenceHandle')
_props('TrackingActionSequenceHandle')
_props('SteppingActionSequenceHandle')
_props('StackingActionSequenceHandle')
_props('PhysicsListActionSequenceHandle')
_props('SensDetActionSequenceHandle')

_props('Geant4PhysicsListActionSequence')

"""
Helper object to perform stuff, which occurs very often.
I am sick of typing the same over and over again.

@author  M.Frank
@version 1.0

"""
class Simple:
  def __init__(self, kernel,calo='Geant4CalorimeterAction',tracker='Geant4SimpleTrackerAction'):
    kernel.UI = "UI"
    kernel.printProperties()
    self.kernel = kernel
    self.lcdd = self.kernel.lcdd()
    self.calo = calo
    self.tracker = tracker
    self.sensitive_types = {}
    self.sensitive_types['tracker'] = self.tracker
    self.sensitive_types['calorimeter'] = self.calo
  def printDetectors(self):
    print '+++  List of sensitive detectors:'
    for i in self.lcdd.detectors():
      o = DetElement(i.second)
      sd = self.lcdd.sensitiveDetector(o.name())
      if sd.isValid():
        typ = sd.type()
        sdtyp = self.sensitive_types[typ]
        print '+++  %-32s type:%-12s  --> Sensitive type: %s'%(o.name(), typ, sdtyp,)

  def setupDetector(self,name,sensitive_type):
    seq = SensitiveSequence(self.kernel,'Geant4SensDetActionSequence/'+name)
    act = SensitiveAction(self.kernel,sensitive_type+'/'+name+'Handler',name)
    seq.add(act)
    return (seq,act)

  def setupCalorimeter(self,name,type=None):
    sd = self.lcdd.sensitiveDetector(name)
    sd.setType('calorimeter')
    if type is None: type = self.calo
    return self.setupDetector(name,type)

  def setupTracker(self,name,type=None):
    sd = self.lcdd.sensitiveDetector(name)
    sd.setType('tracker')
    if type is None: type = self.tracker
    return self.setupDetector(name,type)

  def setupPhysics(self,name):
    phys = self.kernel.physicsList()
    phys.extends = name
    phys.decays  = True
    phys.enableUI()
    phys.dump()
    return phys

  def setupGun(self, name, particle, energy, isotrop=True, multiplicity=1, position=(0.0,0.0,0.0)):
    gun = GeneratorAction(self.kernel,"Geant4ParticleGun/"+name)
    gun.energy   = energy
    gun.particle = particle
    gun.multiplicity = multiplicity
    gun.position = position
    gun.isotrop = isotrop
    gun.enableUI()
    self.kernel.generatorAction().add(gun)
    return gun

  def setupUI(self,typ='csh',vis=False,ui=True):
    # Configure UI
    ui_action = Action(self.kernel,"Geant4UIManager/UI")
    ui_action.HaveVIS = vis
    ui_action.HaveUI = ui
    ui_action.SessionType = typ
    self.kernel.registerGlobalAction(ui_action)

  def setupCshUI(self,typ='csh',vis=False,ui=True):
    self.setupUI(typ='csh',vis=vis,ui=ui)

  def setupROOTOutput(self,name,output):
    evt_root = EventAction(self.kernel,'Geant4Output2ROOT/'+name)
    evt_root.Control = True
    evt_root.Output = output+'.root'
    evt_root.enableUI()
    self.kernel.eventAction().add(evt_root)
    return evt_root

  def setupLCIOOutput(self,name,output):
    evt_lcio = EventAction(self.kernel,'Geant4Output2LCIO/'+name)
    evt_lcio.Control = True
    evt_lcio.Output = output
    evt_lcio.enableUI()
    self.kernel.eventAction().add(evt_lcio)
    return evt_lcio
