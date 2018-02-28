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
import logging
from DD4hep import *

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
def loadDDRec():
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

  result = gSystem.Load("libDDRecPlugins")
  if result < 0:
    raise Exception('DDRec.py: Failed to load the DDRec library libDDRecPlugins: '+gSystem.GetErrorStr())
  from ROOT import dd4hep as module
  return module

# We are nearly there ....
current = __import__(__name__)
def _import_class(ns,nam):  
  scope = getattr(current,ns)
  setattr(current,nam,getattr(scope,nam))

#---------------------------------------------------------------------------
#
try:
  dd4hep     = loadDDRec() 
except Exception as X:
  logging.info('+--%-100s--+',100*'-')
  logging.info('|  %-100s  |','Failed to load DDRec library:')
  logging.info('|  %-100s  |',str(X))
  logging.info('+--%-100s--+',100*'-')
  exit(1)

###FG from ROOT import CLHEP as CLHEP
###FG Core       = dd4hep
###FG Sim        = dd4hep.sim
###FG Simulation = dd4hep.sim
###FG Kernel     = Sim.KernelHandle
###FG Interface  = Sim.Geant4ActionCreation
###FG Detector   = Core.Detector
###FG from DD4hep import std, std_vector, std_list, std_map, std_pair
###FG 
###FG #---------------------------------------------------------------------------
###FG def _constant(self,name):
###FG   return self.constantAsString(name)
###FG 
###FG Detector.globalVal = _constant
###FG #---------------------------------------------------------------------------
###FG 
###FG """
###FG   Import the Detector constants into the DDRec namespace
###FG """
###FG def importConstants(description,namespace=None,debug=False):
###FG   scope = current
###FG   ns = current
###FG   if namespace is not None and not hasattr(current,namespace):
###FG     import imp
###FG     m = imp.new_module('DDRec.'+namespace)
###FG     setattr(current,namespace,m)
###FG     ns = m
###FG   evaluator = dd4hep.g4Evaluator()
###FG   cnt = 0
###FG   num = 0
###FG   todo = {}
###FG   strings = {}
###FG   for c in description.constants():
###FG     if c.second.dataType == 'string':
###FG       strings[c.first] = c.second.GetTitle()
###FG     else:
###FG       todo[c.first] = c.second.GetTitle().replace('(int)','')
###FG   while len(todo) and cnt<100:
###FG     cnt = cnt + 1
###FG     if cnt == 100:
###FG       logging.info('%s %d out of %d %s "%s": [%s]\n+++ %s',\
###FG             '+++ FAILED to import',
###FG             len(todo),len(todo)+num,
###FG             'global values into namespace',
###FG             ns.__name__,'Try to continue anyway',100*'=')
###FG       for k,v in todo.items():
###FG         if not hasattr(ns,k):
###FG           logging.info('+++ FAILED to import: "'+k+'" = "'+str(v)+'"')
###FG       logging.info('+++ %s',100*'=')
###FG 
###FG     for k,v in todo.items():
###FG       if not hasattr(ns,k):
###FG         val = evaluator.evaluate(v)
###FG         status = evaluator.status()
###FG         if status == 0:
###FG           evaluator.setVariable(k,val)
###FG           setattr(ns,k,val)
###FG           if debug: logging.info('Imported global value: "'+k+'" = "'+str(val)+'" into namespace'+ns.__name__)
###FG           del todo[k]
###FG           num = num + 1
###FG   if cnt<100:
###FG     logging.info('+++ Imported %d global values to namespace:%s',num,ns.__name__,)
###FG 
###FG #---------------------------------------------------------------------------  
###FG def _registerGlobalAction(self,action):
###FG   self.get().registerGlobalAction(Interface.toAction(action))
###FG def _registerGlobalFilter(self,filter):
###FG   self.get().registerGlobalFilter(Interface.toAction(filter))
###FG #---------------------------------------------------------------------------
###FG def _getKernelProperty(self, name):
###FG   import exceptions
###FG   #logging.info('_getKernelProperty: %s  %s',str(type(self)),name)
###FG   ret = Interface.getPropertyKernel(self.get(),name)
###FG   if ret.status > 0:
###FG     return ret.data
###FG   elif hasattr(self.get(),name):
###FG     return getattr(self.get(),name)
###FG   elif hasattr(self,name):
###FG     return getattr(self,name)
###FG   msg = 'Geant4Kernel::GetProperty [Unhandled]: Cannot access Kernel.'+name
###FG   raise exceptions.KeyError(msg)
###FG 
###FG #---------------------------------------------------------------------------
###FG def _setKernelProperty(self, name, value):
###FG   import exceptions
###FG   #logging.info('_setKernelProperty: %s %s',name,str(value))
###FG   if Interface.setPropertyKernel(self.get(),name,str(value)):
###FG     return
###FG   msg = 'Geant4Kernel::SetProperty [Unhandled]: Cannot set Kernel.'+name+' = '+str(value)
###FG   raise exceptions.KeyError(msg)
###FG 
###FG #---------------------------------------------------------------------------
###FG def _kernel_phase(self,name):        return self.addSimplePhase(name,False)
###FG #---------------------------------------------------------------------------
###FG def _kernel_worker(self):            return Kernel(self.get().createWorker())
###FG #---------------------------------------------------------------------------
###FG def _kernel_terminate(self):         return self.get().terminate()
###FG #---------------------------------------------------------------------------
###FG Kernel.phase = _kernel_phase
###FG Kernel.registerGlobalAction = _registerGlobalAction
###FG Kernel.registerGlobalFilter = _registerGlobalFilter
###FG Kernel.createWorker = _kernel_worker
###FG Kernel.__getattr__ = _getKernelProperty
###FG Kernel.__setattr__ = _setKernelProperty
###FG Kernel.terminate = _kernel_terminate
###FG #---------------------------------------------------------------------------
###FG ActionHandle = Sim.ActionHandle
###FG #---------------------------------------------------------------------------
###FG def SensitiveAction(kernel,nam,det,shared=False):
###FG   return Interface.createSensitive(kernel,nam,det,shared)
###FG #---------------------------------------------------------------------------
###FG def Action(kernel,nam,shared=False):
###FG   return Interface.createAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def Filter(kernel,nam,shared=False):
###FG   return Interface.createFilter(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def PhaseAction(kernel,nam,shared=False):
###FG   return Interface.createPhaseAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def RunAction(kernel,nam,shared=False):
###FG   return Interface.createRunAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def EventAction(kernel,nam,shared=False):
###FG   return Interface.createEventAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def GeneratorAction(kernel,nam,shared=False):
###FG   return Interface.createGeneratorAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def TrackingAction(kernel,nam,shared=False):
###FG   return Interface.createTrackingAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def SteppingAction(kernel,nam,shared=False):
###FG   return Interface.createSteppingAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def StackingAction(kernel,nam,shared=False):
###FG   return Interface.createStackingAction(kernel,nam,shared)
###FG #---------------------------------------------------------------------------
###FG def DetectorConstruction(kernel,nam):
###FG   return Interface.createDetectorConstruction(kernel,nam)
###FG #---------------------------------------------------------------------------
###FG def PhysicsList(kernel,nam):
###FG   return Interface.createPhysicsList(kernel,nam)
###FG #---------------------------------------------------------------------------
###FG def UserInitialization(kernel, nam):
###FG   return Interface.createUserInitialization(kernel,nam)
###FG #---------------------------------------------------------------------------
###FG def SensitiveSequence(kernel, nam):
###FG   return Interface.createSensDetSequence(kernel,nam)
###FG #---------------------------------------------------------------------------
###FG def _setup(obj):
###FG   def _adopt(self,action):  self.__adopt(action.get())
###FG   _import_class('Sim',obj)
###FG   o = getattr(current,obj)
###FG   setattr(o,'__adopt',getattr(o,'adopt'))
###FG   setattr(o,'adopt',_adopt)
###FG   setattr(o,'add',_adopt)
###FG 
###FG def _setup_callback(obj):
###FG   def _adopt(self,action):  self.__adopt(action.get(),action.callback())
###FG   _import_class('Sim',obj)
###FG   o = getattr(current,obj)
###FG   setattr(o,'__adopt',getattr(o,'add'))
###FG   setattr(o,'add',_adopt)
###FG 
###FG #---------------------------------------------------------------------------
###FG _setup_callback('Geant4ActionPhase')
###FG _setup('Geant4RunActionSequence')
###FG _setup('Geant4EventActionSequence')
###FG _setup('Geant4GeneratorActionSequence')
###FG _setup('Geant4TrackingActionSequence')
###FG _setup('Geant4SteppingActionSequence')
###FG _setup('Geant4StackingActionSequence')
###FG _setup('Geant4PhysicsListActionSequence')
###FG _setup('Geant4SensDetActionSequence')
###FG _setup('Geant4DetectorConstructionSequence')
###FG _setup('Geant4UserInitializationSequence')
###FG _setup('Geant4Sensitive')
###FG _setup('Geant4ParticleHandler')
###FG _import_class('Sim','Geant4Vertex')
###FG _import_class('Sim','Geant4Particle')
###FG _import_class('Sim','Geant4VertexVector')
###FG _import_class('Sim','Geant4ParticleVector')
###FG _import_class('Sim','Geant4Action')
###FG _import_class('Sim','Geant4Filter')
###FG _import_class('Sim','Geant4RunAction')
###FG _import_class('Sim','Geant4TrackingAction')
###FG _import_class('Sim','Geant4StackingAction')
###FG _import_class('Sim','Geant4PhaseAction')
###FG _import_class('Sim','Geant4UserParticleHandler')
###FG _import_class('Sim','Geant4UserInitialization')
###FG _import_class('Sim','Geant4DetectorConstruction')
###FG _import_class('Sim','Geant4GeneratorWrapper')
###FG _import_class('Sim','Geant4Random')
###FG _import_class('CLHEP','HepRandom')
###FG _import_class('CLHEP','HepRandomEngine')
###FG 
###FG #---------------------------------------------------------------------------
###FG def _get(self, name):
###FG   import exceptions, traceback
###FG   #logging.info('_get: %s  %s',str(type(self)),name)
###FG   a = Interface.toAction(self)
###FG   ret = Interface.getProperty(a,name)
###FG   if ret.status > 0:
###FG     return ret.data
###FG   elif hasattr(self.action,name):
###FG     return getattr(self.action,name)
###FG   elif hasattr(a,name):
###FG     return getattr(a,name)
###FG   #elif hasattr(self,name):
###FG   #  return getattr(self,name)
###FG   #traceback.print_stack()
###FG   msg = 'Geant4Action::GetProperty [Unhandled]: Cannot access property '+a.name()+'.'+name
###FG   raise exceptions.KeyError(msg)
###FG 
###FG def _set(self, name, value):
###FG   import exceptions
###FG   #logging.info('_set: %s %s',name,str(value))
###FG   a = Interface.toAction(self)
###FG   if Interface.setProperty(a,name,str(value)):
###FG     return
###FG   msg = 'Geant4Action::SetProperty [Unhandled]: Cannot set '+a.name()+'.'+name+' = '+str(value)
###FG   raise exceptions.KeyError(msg)
###FG 
###FG def _props(obj):
###FG   _import_class('Sim',obj)
###FG   cl = getattr(current,obj)
###FG   cl.__getattr__ = _get
###FG   cl.__setattr__ = _set
###FG 
###FG _props('FilterHandle')
###FG _props('ActionHandle')
###FG _props('PhaseActionHandle')
###FG _props('RunActionHandle')
###FG _props('EventActionHandle')
###FG _props('GeneratorActionHandle')
###FG _props('PhysicsListHandle')
###FG _props('TrackingActionHandle')
###FG _props('SteppingActionHandle')
###FG _props('StackingActionHandle')
###FG _props('DetectorConstructionHandle')
###FG _props('SensitiveHandle')
###FG _props('UserInitializationHandle')
###FG _props('Geant4ParticleHandler')
###FG _props('Geant4UserParticleHandler')
###FG 
###FG _props('GeneratorActionSequenceHandle')
###FG _props('RunActionSequenceHandle')
###FG _props('EventActionSequenceHandle')
###FG _props('TrackingActionSequenceHandle')
###FG _props('SteppingActionSequenceHandle')
###FG _props('StackingActionSequenceHandle')
###FG _props('DetectorConstructionSequenceHandle')
###FG _props('PhysicsListActionSequenceHandle')
###FG _props('SensDetActionSequenceHandle')
###FG _props('UserInitializationSequenceHandle')
###FG 
###FG _props('Geant4PhysicsListActionSequence')
###FG 
###FG """
###FG    Helper object to perform stuff, which occurs very often.
###FG    I am sick of typing the same over and over again.
###FG    Hence, I grouped often used python fragments to this small
###FG    class to re-usage.
###FG 
###FG    Long live laziness!
###FG 
###FG 
###FG    \author  M.Frank
###FG    \version 1.0
###FG 
###FG """
###FG class Geant4:
###FG   def __init__(self, kernel=None,
###FG                calo='Geant4CalorimeterAction',
###FG                tracker='Geant4SimpleTrackerAction'):
###FG     kernel.UI = "UI"
###FG     kernel.printProperties()
###FG     self._kernel = kernel
###FG     if kernel is None:
###FG       self._kernel = Kernel()
###FG     self.description = self._kernel.detectorDescription()
###FG     self.sensitive_types = {}
###FG     self.sensitive_types['tracker'] = tracker
###FG     self.sensitive_types['calorimeter'] = calo
###FG     self.sensitive_types['escape_counter'] = 'Geant4EscapeCounter'
###FG 
###FG 
###FG   def kernel(self):
###FG     return self._kernel.worker()
###FG   def master(self):
###FG     return self._kernel
###FG 
###FG   """
###FG      Configure the Geant4 command executive
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def setupUI(self,typ='csh',vis=False,ui=True,macro=None):
###FG     # Configure UI
###FG     ui_action = Action(self.master(),"Geant4UIManager/UI")
###FG     if vis:      ui_action.HaveVIS = True
###FG     else:        ui_action.HaveVIS = False
###FG     if ui:       ui_action.HaveUI  = True
###FG     else:        ui_action.HaveUI  = False
###FG     ui_action.SessionType = typ
###FG     if macro:
###FG       ui_action.SetupUI = macro
###FG     self.master().registerGlobalAction(ui_action)
###FG     return ui_action
###FG 
###FG   """
###FG      Configure the Geant4 command executive with a csh like command prompt
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def setupCshUI(self,typ='csh',vis=False,ui=True,macro=None):
###FG     return self.setupUI(typ='csh',vis=vis,ui=ui,macro=macro)
###FG 
###FG   """
###FG      Configure Geant4 user initialization for optionasl multi-threading mode
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addUserInitialization(self, worker, worker_args=None, master=None, master_args=None):
###FG     import sys
###FG     init_seq = self.master().userInitialization(True)
###FG     init_action = UserInitialization(self.master(),'Geant4PythonInitialization/PyG4Init')
###FG     #
###FG     if worker:
###FG       init_action.setWorkerSetup(worker, worker_args)
###FG     else:
###FG       raise exceptions.RuntimeError('Invalid argument for Geant4 worker initialization')
###FG     #
###FG     if master:
###FG       init_action.setMasterSetup(master,master_args)
###FG     #
###FG     init_seq.adopt(init_action)
###FG     return init_seq,init_action
###FG 
###FG   """
###FG      Configure Geant4 user initialization for optionasl multi-threading mode
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addDetectorConstruction(self,            name_type,
###FG                               field=None,      field_args=None,
###FG                               geometry=None,   geometry_args=None,
###FG                               sensitives=None, sensitives_args=None,
###FG                               allow_threads=False):
###FG     init_seq = self.master().detectorConstruction(True)
###FG     init_action = DetectorConstruction(self.master(),name_type)
###FG     #
###FG     if geometry:
###FG       init_action.setConstructGeo(geometry,geometry_args)
###FG     #
###FG     if field:
###FG       init_action.setConstructField(field,field_args)
###FG     #
###FG     if sensitives:
###FG       init_action.setConstructSensitives(sensitives,sensitives_args)
###FG     #
###FG     init_seq.adopt(init_action)
###FG     if allow_threads:
###FG       last_action = DetectorConstruction(self.master(),"Geant4PythonDetectorConstructionLast/LastDetectorAction")
###FG       init_seq.adopt(last_action)
###FG 
###FG     return init_seq,init_action
###FG 
###FG   """
###FG      Add a new phase action to an arbitrary step.
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addPhaseAction(self,phase_name,factory_specification,ui=True,instance=None):
###FG     if instance is None:
###FG       instance = self.kernel()
###FG     action = PhaseAction(instance,factory_specification)
###FG     instance.phase(phase_name).add(action)
###FG     if ui: action.enableUI()
###FG     return action
###FG 
###FG   """
###FG      Add a new phase action to the 'configure' step.
###FG      Called at the beginning of Geant4Exec::configure.
###FG      The factory specification is the typical string "<factory_name>/<instance name>".
###FG      If no instance name is specified it defaults to the factory name.
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addConfig(self, factory_specification):
###FG     return self.addPhaseAction('configure',factory_specification,instance=self.master())
###FG 
###FG   """
###FG      Add a new phase action to the 'initialize' step.
###FG      Called at the beginning of Geant4Exec::initialize.
###FG      The factory specification is the typical string "<factory_name>/<instance name>".
###FG      If no instance name is specified it defaults to the factory name.
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addInit(self, factory_specification):
###FG     return self.addPhaseAction('initialize',factory_specification)
###FG 
###FG   """
###FG      Add a new phase action to the 'start' step.
###FG      Called at the beginning of Geant4Exec::run.
###FG      The factory specification is the typical string "<factory_name>/<instance name>".
###FG      If no instance name is specified it defaults to the factory name.
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addStart(self, factory_specification):
###FG     return self.addPhaseAction('start',factory_specification)
###FG 
###FG   """
###FG      Add a new phase action to the 'stop' step.
###FG      Called at the end of Geant4Exec::run.
###FG      The factory specification is the typical string "<factory_name>/<instance name>".
###FG      If no instance name is specified it defaults to the factory name.
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def addStop(self, factory_specification):
###FG     return self.addPhaseAction('stop',factory_specification)
###FG 
###FG   """
###FG      Execute the Geant 4 program with all steps.
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def execute(self):
###FG     self.kernel().configure()
###FG     self.kernel().initialize()
###FG     self.kernel().run()
###FG     self.kernel().terminate()
###FG     return self
###FG 
###FG   def printDetectors(self):
###FG     logging.info('+++  List of sensitive detectors:')
###FG     for i in self.description.detectors():
###FG       #logging.info(i.second.ptr().GetName())
###FG       o = DetElement(i.second.ptr())
###FG       sd = self.description.sensitiveDetector(o.name())
###FG       if sd.isValid():
###FG         typ = sd.type()
###FG         sdtyp = 'Unknown'
###FG         if self.sensitive_types.has_key(typ):
###FG           sdtyp = self.sensitive_types[typ]
###FG         logging.info('+++  %-32s type:%-12s  --> Sensitive type: %s',o.name(), typ, sdtyp)
###FG 
###FG   def setupSensitiveSequencer(self, name, action):
###FG     if isinstance( action, tuple ):
###FG       sensitive_type = action[0]
###FG     else:
###FG       sensitive_type = action
###FG     seq = SensitiveSequence(self.kernel(),'Geant4SensDetActionSequence/'+name)
###FG     seq.enableUI()
###FG     return seq
###FG    
###FG   def setupDetector(self,name,action,collections=None):
###FG     #fg: allow the action to be a tuple with parameter dictionary
###FG     sensitive_type = ""
###FG     parameterDict = {}
###FG     if isinstance(action,tuple) or isinstance(action,list):
###FG       sensitive_type = action[0]
###FG       parameterDict = action[1]
###FG     else:
###FG       sensitive_type = action
###FG 
###FG     seq = SensitiveSequence(self.kernel(),'Geant4SensDetActionSequence/'+name)
###FG     seq.enableUI()
###FG     acts = []
###FG     if collections is None:
###FG       sd = self.description.sensitiveDetector(name)
###FG       ro = sd.readout()
###FG       collections = ro.collectionNames()
###FG       if len(collections)==0:
###FG         act = SensitiveAction(self.kernel(),sensitive_type+'/'+name+'Handler',name)
###FG         for parameter, value in parameterDict.iteritems():
###FG           setattr( act, parameter, value)
###FG         acts.append(act)
###FG 
###FG     # Work down the collections if present
###FG     if collections is not None:
###FG       for coll in collections:
###FG         params = {}
###FG         if isinstance(coll,tuple) or isinstance(coll,list):
###FG           if len(coll)>2:
###FG             coll_nam = coll[0]
###FG             sensitive_type = coll[1]
###FG             params = coll[2]
###FG           elif len(coll)>1:
###FG             coll_nam = coll[0]
###FG             sensitive_type = coll[1]
###FG           else:
###FG             coll_nam = coll[0]
###FG         else:
###FG           coll_nam = coll
###FG         act = SensitiveAction(self.kernel(),sensitive_type+'/'+coll_nam+'Handler',name)
###FG         act.CollectionName = coll_nam
###FG         for parameter, value in params.iteritems():
###FG           setattr( act, parameter, value)
###FG         acts.append(act)
###FG 
###FG     for act in acts:
###FG       act.enableUI()
###FG       seq.add(act)
###FG     if len(acts)>1:
###FG       return (seq,acts)
###FG     return (seq,acts[0])
###FG 
###FG   def setupCalorimeter(self,name,type=None,collections=None):
###FG     sd = self.description.sensitiveDetector(name)
###FG     sd.setType('calorimeter')
###FG     if type is None: type = self.sensitive_types['calorimeter']
###FG     return self.setupDetector(name,type,collections)
###FG 
###FG   def setupTracker(self,name,type=None,collections=None):
###FG     sd = self.description.sensitiveDetector(name)
###FG     sd.setType('tracker')
###FG     if type is None: type = self.sensitive_types['tracker']
###FG     return self.setupDetector(name,type,collections)
###FG 
###FG   def _private_setupField(self, field, stepper, equation, prt):
###FG     import SystemOfUnits
###FG     field.stepper            = stepper
###FG     field.equation           = equation
###FG     field.eps_min            = 5e-05*SystemOfUnits.mm
###FG     field.eps_max            = 0.001*SystemOfUnits.mm
###FG     field.min_chord_step     = 0.01*SystemOfUnits.mm
###FG     field.delta_chord        = 0.25*SystemOfUnits.mm
###FG     field.delta_intersection = 0.001*SystemOfUnits.mm
###FG     field.delta_one_step     = 0.01*SystemOfUnits.mm
###FG     field.largest_step       = 1000*SystemOfUnits.m
###FG     if prt:
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> stepper  = ',str(field.stepper),'')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> equation = ',str(field.equation),'')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> eps_min  = ',str(field.eps_min),'[mm]')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> eps_max  = ',str(field.eps_max),'[mm]')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> delta_chord        = ',str(field.delta_chord),'[mm]')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> min_chord_step     = ',str(field.min_chord_step),'[mm]')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> delta_one_step     = ',str(field.delta_one_step),'[mm]')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> delta_intersection = ',str(field.delta_intersection),'[mm]')
###FG       logging.info('+++++> %s %s %s %s ',field.name,'-> largest_step       = ',str(field.largest_step),'[mm]')
###FG     return field
###FG     
###FG   def setupTrackingFieldMT(self, name='MagFieldTrackingSetup', stepper='G4ClassicalRK4', equation='Mag_UsualEqRhs',prt=False):
###FG     seq,fld = self.addDetectorConstruction("Geant4FieldTrackingConstruction/"+name)
###FG     self._private_setupField(fld, stepper, equation, prt)
###FG     return (seq,fld)
###FG 
###FG   def setupTrackingField(self, name='MagFieldTrackingSetup', stepper='G4ClassicalRK4', equation='Mag_UsualEqRhs',prt=False):
###FG     field = self.addConfig('Geant4FieldTrackingSetupAction/'+name)
###FG     self._private_setupField(field, stepper, equation, prt)
###FG     return field
###FG   
###FG   def setupPhysics(self, name):
###FG     phys = self.master().physicsList()
###FG     phys.extends = name
###FG     phys.decays  = True
###FG     phys.enableUI()
###FG     phys.dump()
###FG     return phys
###FG 
###FG   def addPhysics(self, name):
###FG     phys = self.master().physicsList()
###FG     opt  = PhysicsList(self.master(), name)
###FG     opt.enableUI()
###FG     phys.adopt(opt)
###FG     return opt
###FG   
###FG   def setupGun(self, name, particle, energy, isotrop=True, multiplicity=1, position=(0.0,0.0,0.0),register=True, **args):
###FG     gun = GeneratorAction(self.kernel(),"Geant4ParticleGun/"+name,True)
###FG     for i in args.items():
###FG       setattr(gun,i[0],i[1])
###FG     gun.energy       = energy
###FG     gun.particle     = particle
###FG     gun.multiplicity = multiplicity
###FG     gun.position     = position
###FG     gun.isotrop      = isotrop
###FG     gun.enableUI()
###FG     if register:
###FG       self.kernel().generatorAction().add(gun)
###FG     return gun
###FG 
###FG   """
###FG      Configure ROOT output for the simulated events
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def setupROOTOutput(self,name,output,mc_truth=True):
###FG     evt_root = EventAction(self.kernel(),'Geant4Output2ROOT/'+name,True)
###FG     evt_root.HandleMCTruth = mc_truth
###FG     evt_root.Control = True
###FG     if not output.endswith('.root'):
###FG       output = output + '.root'
###FG     evt_root.Output = output
###FG     evt_root.enableUI()
###FG     self.kernel().eventAction().add(evt_root)
###FG     return evt_root
###FG 
###FG   """
###FG      Configure LCIO output for the simulated events
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def setupLCIOOutput(self,name,output):
###FG     evt_lcio = EventAction(self.kernel(),'Geant4Output2LCIO/'+name,True)
###FG     evt_lcio.Control = True
###FG     evt_lcio.Output  = output
###FG     evt_lcio.enableUI()
###FG     self.kernel().eventAction().add(evt_lcio)
###FG     return evt_lcio
###FG 
###FG   """
###FG      Generic build of the input stage with multiple input modules.
###FG 
###FG      Actions executed are:
###FG      1) Register Generation initialization action
###FG      2) Append all modules to build the complete input record
###FG         These modules are readers/particle sources, boosters and/or smearing actions.
###FG      3) Merge all existing interaction records
###FG      4) Add the MC truth handler
###FG 
###FG      \author  M.Frank
###FG   """
###FG   def buildInputStage(self, generator_input_modules, output_level=None, have_mctruth=True):
###FG     ga = self.kernel().generatorAction()
###FG     # Register Generation initialization action
###FG     gen = GeneratorAction(self.kernel(),"Geant4GeneratorActionInit/GenerationInit")
###FG     if output_level is not None:
###FG       gen.OutputLevel = output_level
###FG     ga.adopt(gen)
###FG 
###FG     # Now append all modules to build the complete input record
###FG     # These modules are readers/particle sources, boosters and/or smearing actions
###FG     for gen in generator_input_modules:
###FG       gen.enableUI()
###FG       if output_level is not None:
###FG         gen.OutputLevel = output_level
###FG       ga.adopt(gen)
###FG 
###FG     # Merge all existing interaction records
###FG     gen = GeneratorAction(self.kernel(),"Geant4InteractionMerger/InteractionMerger")
###FG     gen.enableUI()
###FG     if output_level is not None:
###FG       gen.OutputLevel = output_level
###FG     ga.adopt(gen)
###FG 
###FG     # Finally generate Geant4 primaries
###FG     if have_mctruth:
###FG       gen = GeneratorAction(self.kernel(),"Geant4PrimaryHandler/PrimaryHandler")
###FG       gen.enableUI()
###FG       if output_level is not None:
###FG         gen.OutputLevel = output_level
###FG       ga.adopt(gen)
###FG     # Puuuhh! All done.
###FG     return self
###FG 
###FG   """
###FG      Execute the main Geant4 action
###FG      \author  M.Frank
###FG   """
###FG   def run(self):
###FG     #self.master().configure()
###FG     #self.master().initialize()
###FG     #self.master().run()
###FG     #self.master().terminate()
###FG     from ROOT import PyDDRec
###FG     PyDDRec.run(self.master().get())
###FG     return self
###FG 
###FG Simple = Geant4
