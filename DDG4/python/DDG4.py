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

from DD4hep import *

def loadDDG4():
  ## import ROOT ## done in import * above
  from ROOT import gSystem

  ## Try to load libglapi to avoid issues with TLS Static
  ## Turn off all errors from ROOT about the library missing
  orgLevel = ROOT.gErrorIgnoreLevel
  ROOT.gErrorIgnoreLevel=6000
  gSystem.Load("libglapi")
  ROOT.gErrorIgnoreLevel=orgLevel

  import platform
  import os
  if platform.system()=="Darwin":
    gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])

  result = gSystem.Load("libDDG4Plugins")
  if 0 != result:
    raise Exception('DDG4.py: Failed to load the Geant4 library libDDG4: '+gSystem.GetErrorStr())
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
except Exception as X:
  print '+--%-100s--+'%(100*'-',)
  print '|  %-100s  |'%('Failed to load DDG4 library:',)
  print '|  %-100s  |'%(str(X),)
  print '+--%-100s--+'%(100*'-',)
  exit(1)
from ROOT import CLHEP as CLHEP
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
  strings = {}
  for c in lcdd.constants():
    if c.second.dataType == 'string':
      strings[c.first] = c.second.GetTitle()
    else:
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

#---------------------------------------------------------------------------
def _kernel_phase(self,name):        return self.addSimplePhase(name,False)
#---------------------------------------------------------------------------
def _kernel_worker(self):            return Kernel(self.get().createWorker())
#---------------------------------------------------------------------------
def _kernel_terminate(self):         return self.get().terminate()
#---------------------------------------------------------------------------
Kernel.phase = _kernel_phase
Kernel.registerGlobalAction = _registerGlobalAction
Kernel.registerGlobalFilter = _registerGlobalFilter
Kernel.createWorker = _kernel_worker
Kernel.__getattr__ = _getKernelProperty
Kernel.__setattr__ = _setKernelProperty
Kernel.terminate = _kernel_terminate
#---------------------------------------------------------------------------
ActionHandle = Sim.ActionHandle
#---------------------------------------------------------------------------
def SensitiveAction(kernel,nam,det,shared=False):
  return Interface.createSensitive(kernel,nam,det,shared)
#---------------------------------------------------------------------------
def Action(kernel,nam,shared=False):
  return Interface.createAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def Filter(kernel,nam,shared=False):
  return Interface.createFilter(kernel,nam,shared)
#---------------------------------------------------------------------------
def PhaseAction(kernel,nam,shared=False):
  return Interface.createPhaseAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def RunAction(kernel,nam,shared=False):
  return Interface.createRunAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def EventAction(kernel,nam,shared=False):
  return Interface.createEventAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def GeneratorAction(kernel,nam,shared=False):
  return Interface.createGeneratorAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def TrackingAction(kernel,nam,shared=False):
  return Interface.createTrackingAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def SteppingAction(kernel,nam,shared=False):
  return Interface.createSteppingAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def StackingAction(kernel,nam,shared=False):
  return Interface.createStackingAction(kernel,nam,shared)
#---------------------------------------------------------------------------
def DetectorConstruction(kernel,nam):
  return Interface.createDetectorConstruction(kernel,nam)
#---------------------------------------------------------------------------
def PhysicsList(kernel,nam):
  return Interface.createPhysicsList(kernel,nam)
#---------------------------------------------------------------------------
def UserInitialization(kernel, nam):
  return Interface.createUserInitialization(kernel,nam)
#---------------------------------------------------------------------------
def SensitiveSequence(kernel, nam):
  return Interface.createSensDetSequence(kernel,nam)
#---------------------------------------------------------------------------
def _setup(obj):
  def _adopt(self,action):  self.__adopt(action.get())
  _import_class('Sim',obj)
  o = getattr(current,obj)
  setattr(o,'__adopt',getattr(o,'adopt'))
  setattr(o,'adopt',_adopt)
  setattr(o,'add',_adopt)

def _setup_callback(obj):
  def _adopt(self,action):  self.__adopt(action.get(),action.callback())
  _import_class('Sim',obj)
  o = getattr(current,obj)
  setattr(o,'__adopt',getattr(o,'add'))
  setattr(o,'add',_adopt)

#---------------------------------------------------------------------------
_setup_callback('Geant4ActionPhase')
_setup('Geant4RunActionSequence')
_setup('Geant4EventActionSequence')
_setup('Geant4GeneratorActionSequence')
_setup('Geant4TrackingActionSequence')
_setup('Geant4SteppingActionSequence')
_setup('Geant4StackingActionSequence')
_setup('Geant4PhysicsListActionSequence')
_setup('Geant4SensDetActionSequence')
_setup('Geant4DetectorConstructionSequence')
_setup('Geant4UserInitializationSequence')
_setup('Geant4Sensitive')
_setup('Geant4ParticleHandler')
_import_class('Sim','Geant4Vertex')
_import_class('Sim','Geant4Particle')
_import_class('Sim','Geant4VertexVector')
_import_class('Sim','Geant4ParticleVector')
_import_class('Sim','Geant4Action')
_import_class('Sim','Geant4Filter')
_import_class('Sim','Geant4RunAction')
_import_class('Sim','Geant4TrackingAction')
_import_class('Sim','Geant4StackingAction')
_import_class('Sim','Geant4PhaseAction')
_import_class('Sim','Geant4UserParticleHandler')
_import_class('Sim','Geant4UserInitialization')
_import_class('Sim','Geant4DetectorConstruction')
_import_class('Sim','Geant4GeneratorWrapper')
_import_class('Sim','Geant4Random')
_import_class('CLHEP','HepRandom')
_import_class('CLHEP','HepRandomEngine')

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
_props('PhaseActionHandle')
_props('RunActionHandle')
_props('EventActionHandle')
_props('GeneratorActionHandle')
_props('PhysicsListHandle')
_props('TrackingActionHandle')
_props('SteppingActionHandle')
_props('StackingActionHandle')
_props('DetectorConstructionHandle')
_props('SensitiveHandle')
_props('UserInitializationHandle')
_props('Geant4ParticleHandler')
_props('Geant4UserParticleHandler')

_props('GeneratorActionSequenceHandle')
_props('RunActionSequenceHandle')
_props('EventActionSequenceHandle')
_props('TrackingActionSequenceHandle')
_props('SteppingActionSequenceHandle')
_props('StackingActionSequenceHandle')
_props('DetectorConstructionSequenceHandle')
_props('PhysicsListActionSequenceHandle')
_props('SensDetActionSequenceHandle')
_props('UserInitializationSequenceHandle')

_props('Geant4PhysicsListActionSequence')

"""
   Helper object to perform stuff, which occurs very often.
   I am sick of typing the same over and over again.
   Hence, I grouped often used python fragments to this small
   class to re-usage.

   Long live laziness!


   \author  M.Frank
   \version 1.0

"""
class Geant4:
  def __init__(self, kernel=None,
               calo='Geant4CalorimeterAction',
               tracker='Geant4SimpleTrackerAction'):
    kernel.UI = "UI"
    kernel.printProperties()
    self._kernel = kernel
    if kernel is None:
      self._kernel = Kernel()
    self.lcdd = self._kernel.lcdd()
    self.sensitive_types = {}
    self.sensitive_types['tracker'] = tracker
    self.sensitive_types['calorimeter'] = calo
    self.sensitive_types['escape_counter'] = 'Geant4EscapeCounter'


  def kernel(self):
    return self._kernel.worker()
  def master(self):
    return self._kernel

  """
     Configure the Geant4 command executive

     \author  M.Frank
  """
  def setupUI(self,typ='csh',vis=False,ui=True,macro=None):
    # Configure UI
    ui_action = Action(self.master(),"Geant4UIManager/UI")
    if vis:      ui_action.HaveVIS = True
    else:        ui_action.HaveVIS = False
    if ui:       ui_action.HaveUI  = True
    else:        ui_action.HaveUI  = False
    ui_action.SessionType = typ
    if macro:
      ui_action.SetupUI = macro
    self.master().registerGlobalAction(ui_action)
    return ui_action

  """
     Configure the Geant4 command executive with a csh like command prompt

     \author  M.Frank
  """
  def setupCshUI(self,typ='csh',vis=False,ui=True,macro=None):
    return self.setupUI(typ='csh',vis=vis,ui=ui,macro=macro)

  """
     Configure Geant4 user initialization for optionasl multi-threading mode

     \author  M.Frank
  """
  def addUserInitialization(self, worker, worker_args=None, master=None, master_args=None):
    import sys
    init_seq = self.master().userInitialization(True)
    init_action = UserInitialization(self.master(),'Geant4PythonInitialization/PyG4Init')
    #
    if worker:
      init_action.setWorkerSetup(worker, worker_args)
    else:
      raise exceptions.RuntimeError('Invalid argument for Geant4 worker initialization')
    #
    if master:
      init_action.setMasterSetup(master,master_args)
    #
    init_seq.adopt(init_action)
    return init_seq,init_action

  """
     Configure Geant4 user initialization for optionasl multi-threading mode

     \author  M.Frank
  """
  def addDetectorConstruction(self, name_type,
                              field=None, field_args=None,
                              geometry=None, geometry_args=None,
                              sensitives=None, sensitives_args=None,
                              allow_threads=False):
    init_seq = self.master().detectorConstruction(True)
    init_action = DetectorConstruction(self.master(),name_type)
    #
    if geometry:
      init_action.setConstructGeo(geometry,geometry_args)
    #
    if field:
      init_action.setConstructField(field,field_args)
    #
    if sensitives:
      init_action.setConstructSensitives(sensitives,sensitives_args)
    #
    init_seq.adopt(init_action)
    if allow_threads:
      last_action = DetectorConstruction(self.master(),"Geant4PythonDetectorConstructionLast/LastDetectorAction")
      init_seq.adopt(last_action)

    return init_seq,init_action

  """
     Add a new phase action to an arbitrary step.

     \author  M.Frank
  """
  def addPhaseAction(self,phase_name,factory_specification,ui=True,instance=None):
    if instance is None:
      instance = self.kernel()
    action = PhaseAction(instance,factory_specification)
    instance.phase(phase_name).add(action)
    if ui: action.enableUI()
    return action

  """
     Add a new phase action to the 'configure' step.
     Called at the beginning of Geant4Exec::configure.
     The factory specification is the typical string "<factory_name>/<instance name>".
     If no instance name is specified it defaults to the factory name.

     \author  M.Frank
  """
  def addConfig(self, factory_specification):
    return self.addPhaseAction('configure',factory_specification,instance=self.master())

  """
     Add a new phase action to the 'initialize' step.
     Called at the beginning of Geant4Exec::initialize.
     The factory specification is the typical string "<factory_name>/<instance name>".
     If no instance name is specified it defaults to the factory name.

     \author  M.Frank
  """
  def addInit(self, factory_specification):
    return self.addPhaseAction('initialize',factory_specification)

  """
     Add a new phase action to the 'start' step.
     Called at the beginning of Geant4Exec::run.
     The factory specification is the typical string "<factory_name>/<instance name>".
     If no instance name is specified it defaults to the factory name.

     \author  M.Frank
  """
  def addStart(self, factory_specification):
    return self.addPhaseAction('start',factory_specification)

  """
     Add a new phase action to the 'stop' step.
     Called at the end of Geant4Exec::run.
     The factory specification is the typical string "<factory_name>/<instance name>".
     If no instance name is specified it defaults to the factory name.

     \author  M.Frank
  """
  def addStop(self, factory_specification):
    return self.addPhaseAction('stop',factory_specification)

  """
     Execute the Geant 4 program with all steps.

     \author  M.Frank
  """
  def execute(self):
    self.kernel().configure()
    self.kernel().initialize()
    self.kernel().run()
    self.kernel().terminate()
    return self

  def printDetectors(self):
    print '+++  List of sensitive detectors:'
    for i in self.lcdd.detectors():
      #print i.second.ptr().GetName()
      o = DetElement(i.second.ptr())
      sd = self.lcdd.sensitiveDetector(o.name())
      if sd.isValid():
        typ = sd.type()
        sdtyp = 'Unknown'
        if self.sensitive_types.has_key(typ):
          sdtyp = self.sensitive_types[typ]
        print '+++  %-32s type:%-12s  --> Sensitive type: %s'%(o.name(), typ, sdtyp,)

  def setupSensitiveSequencer(self, name, action):
    if isinstance( action, tuple ):
      sensitive_type = action[0]
    else:
      sensitive_type = action
    seq = SensitiveSequence(self.kernel(),'Geant4SensDetActionSequence/'+name)
    seq.enableUI()
    return seq
   
  def setupDetector(self,name,action,collections=None):
    #fg: allow the action to be a tuple with parameter dictionary
    sensitive_type = ""
    parameterDict = {}
    if isinstance(action,tuple) or isinstance(action,list):
      sensitive_type = action[0]
      parameterDict = action[1]
    else:
      sensitive_type = action

    seq = SensitiveSequence(self.kernel(),'Geant4SensDetActionSequence/'+name)
    seq.enableUI()
    acts = []
    if collections is None:
      sd = self.lcdd.sensitiveDetector(name)
      ro = sd.readout()
      #print dir(ro)
      collections = ro.collectionNames()
      if len(collections)==0:
        act = SensitiveAction(self.kernel(),sensitive_type+'/'+name+'Handler',name)
        for parameter, value in parameterDict.iteritems():
          setattr( act, parameter, value)
        acts.append(act)

    # Work down the collections if present
    if collections is not None:
      for coll in collections:
        params = {}
        if isinstance(coll,tuple) or isinstance(coll,list):
          if len(coll)>2:
            coll_nam = coll[0]
            sensitive_type = coll[1]
            params = coll[2]
          elif len(coll)>1:
            coll_nam = coll[0]
            sensitive_type = coll[1]
          else:
            coll_nam = coll[0]
        else:
          coll_nam = coll
        act = SensitiveAction(self.kernel(),sensitive_type+'/'+coll_nam+'Handler',name)
        act.CollectionName = coll_nam
        for parameter, value in params.iteritems():
          setattr( act, parameter, value)
        acts.append(act)

    for act in acts:
      act.enableUI()
      seq.add(act)
    if len(acts)>1:
      return (seq,acts)
    return (seq,acts[0])

  def setupCalorimeter(self,name,type=None,collections=None):
    sd = self.lcdd.sensitiveDetector(name)
    sd.setType('calorimeter')
    if type is None: type = self.sensitive_types['calorimeter']
    return self.setupDetector(name,type,collections)

  def setupTracker(self,name,type=None,collections=None):
    sd = self.lcdd.sensitiveDetector(name)
    sd.setType('tracker')
    if type is None: type = self.sensitive_types['tracker']
    return self.setupDetector(name,type,collections)

  def setupTrackingField(self, name='MagFieldTrackingSetup', stepper='HelixSimpleRunge', equation='Mag_UsualEqRhs',prt=False):
    import SystemOfUnits
    field = self.addConfig('Geant4FieldTrackingSetupAction/'+name)
    field.stepper            = stepper
    field.equation           = equation
    field.eps_min            = 5e-05*SystemOfUnits.mm
    field.eps_max            = 0.001*SystemOfUnits.mm
    field.min_chord_step     = 0.01*SystemOfUnits.mm
    field.delta_chord        = 0.25*SystemOfUnits.mm
    field.delta_intersection = 1e-05*SystemOfUnits.mm
    field.delta_one_step     = 0.001*SystemOfUnits.mm
    field.largest_step       = 10*SystemOfUnits.m
    if prt:
      print '+++++> ',field.name,'-> stepper  = ',field.stepper
      print '+++++> ',field.name,'-> equation = ',field.equation
      print '+++++> ',field.name,'-> eps_min  = ',field.eps_min,'[mm]'
      print '+++++> ',field.name,'-> eps_max  = ',field.eps_max,'[mm]'
      print '+++++> ',field.name,'-> delta_chord        = ',field.delta_chord,'[mm]'
      print '+++++> ',field.name,'-> min_chord_step     = ',field.min_chord_step,'[mm]'
      print '+++++> ',field.name,'-> delta_one_step     = ',field.delta_one_step,'[mm]'
      print '+++++> ',field.name,'-> delta_intersection = ',field.delta_intersection,'[mm]'
      print '+++++> ',field.name,'-> largest_step       = ',field.largest_step,'[mm]'
    return field
  
  def setupPhysics(self, name):
    phys = self.master().physicsList()
    phys.extends = name
    phys.decays  = True
    phys.enableUI()
    phys.dump()
    return phys

  def addPhysics(self, name):
    phys = self.master().physicsList()
    opt  = PhysicsList(self.master(), name)
    opt.enableUI()
    phys.adopt(opt)
    return opt
  
  def setupGun(self, name, particle, energy, isotrop=True, multiplicity=1, position=(0.0,0.0,0.0),**args):
    gun = GeneratorAction(self.kernel(),"Geant4ParticleGun/"+name,True)
    for i in args.items():
      setattr(gun,i[0],i[1])
    gun.energy       = energy
    gun.particle     = particle
    gun.multiplicity = multiplicity
    gun.position     = position
    gun.isotrop      = isotrop
    gun.enableUI()
    self.kernel().generatorAction().add(gun)
    return gun

  """
     Configure ROOT output for the simulated events

     \author  M.Frank
  """
  def setupROOTOutput(self,name,output,mc_truth=True):
    evt_root = EventAction(self.kernel(),'Geant4Output2ROOT/'+name,True)
    evt_root.HandleMCTruth = mc_truth
    evt_root.Control = True
    if not output.endswith('.root'):
      output = output + '.root'
    evt_root.Output = output
    evt_root.enableUI()
    self.kernel().eventAction().add(evt_root)
    return evt_root

  """
     Configure LCIO output for the simulated events

     \author  M.Frank
  """
  def setupLCIOOutput(self,name,output):
    evt_lcio = EventAction(self.kernel(),'Geant4Output2LCIO/'+name,True)
    evt_lcio.Control = True
    evt_lcio.Output  = output
    evt_lcio.enableUI()
    self.kernel().eventAction().add(evt_lcio)
    return evt_lcio

  """
     Generic build of the input stage with multiple input modules.

     Actions executed are:
     1) Register Generation initialization action
     2) Append all modules to build the complete input record
        These modules are readers/particle sources, boosters and/or smearing actions.
     3) Merge all existing interaction records
     4) Add the MC truth handler

     \author  M.Frank
  """
  def buildInputStage(self, generator_input_modules, output_level=None, have_mctruth=True):
    ga = self.kernel().generatorAction()
    # Register Generation initialization action
    gen = GeneratorAction(self.kernel(),"Geant4GeneratorActionInit/GenerationInit")
    if output_level is not None:
      gen.OutputLevel = output_level
    ga.adopt(gen)

    # Now append all modules to build the complete input record
    # These modules are readers/particle sources, boosters and/or smearing actions
    for gen in generator_input_modules:
      gen.enableUI()
      if output_level is not None:
        gen.OutputLevel = output_level
      ga.adopt(gen)

    # Merge all existing interaction records
    gen = GeneratorAction(self.kernel(),"Geant4InteractionMerger/InteractionMerger")
    gen.enableUI()
    if output_level is not None:
      gen.OutputLevel = output_level
    ga.adopt(gen)

    # Finally generate Geant4 primaries
    if have_mctruth:
      gen = GeneratorAction(self.kernel(),"Geant4PrimaryHandler/PrimaryHandler")
      gen.enableUI()
      if output_level is not None:
        gen.OutputLevel = output_level
      ga.adopt(gen)
    # Puuuhh! All done.
    return self

  """
     Execute the main Geant4 action
     \author  M.Frank
  """
  def run(self):
    #self.master().configure()
    #self.master().initialize()
    #self.master().run()
    #self.master().terminate()
    from ROOT import PyDDG4
    PyDDG4.run(self.master().get())
    return self

Simple = Geant4
