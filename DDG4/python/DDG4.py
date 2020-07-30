# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
from __future__ import absolute_import, unicode_literals
import logging
from dd4hep_base import *  # noqa: F403
import ddsix as six

logger = logging.getLogger(__name__)


def loadDDG4():
  import ROOT
  from ROOT import gSystem

  # Try to load libglapi to avoid issues with TLS Static
  # Turn off all errors from ROOT about the library missing
  if('libglapi' not in gSystem.GetLibraries()):
    orgLevel = ROOT.gErrorIgnoreLevel
    ROOT.gErrorIgnoreLevel = 6000
    gSystem.Load("libglapi")
    ROOT.gErrorIgnoreLevel = orgLevel

  import platform
  import os
  if platform.system() == "Darwin":
    gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])
    os.environ['DYLD_LIBRARY_PATH'] = os.pathsep.join([os.environ['DD4HEP_LIBRARY_PATH'],
                                                       os.environ.get('DYLD_LIBRARY_PATH', '')]).strip(os.pathsep)

  result = gSystem.Load("libDDG4Plugins")
  if result < 0:
    raise Exception('DDG4.py: Failed to load the DDG4 library libDDG4Plugins: ' + gSystem.GetErrorStr())
  from ROOT import dd4hep as module
  return module


# We are nearly there ....
current = __import__(__name__)


def _import_class(ns, nam):
  scope = getattr(current, ns)
  setattr(current, nam, getattr(scope, nam))


try:
  dd4hep = loadDDG4()
except Exception as X:
  logger.error('+--%-100s--+', 100 * '-')
  logger.error('|  %-100s  |', 'Failed to load DDG4 library:')
  logger.error('|  %-100s  |', str(X))
  logger.error('+--%-100s--+', 100 * '-')
  exit(1)

from ROOT import CLHEP as CLHEP  # noqa
Core = dd4hep
Sim = dd4hep.sim
Simulation = dd4hep.sim
Kernel = Sim.KernelHandle
Interface = Sim.Geant4ActionCreation
Detector = Core.Detector


def _constant(self, name):
  return self.constantAsString(name)


Detector.globalVal = _constant


def importConstants(description, namespace=None, debug=False):
  """
  Import the Detector constants into the DDG4 namespace
  """
  ns = current
  if namespace is not None and not hasattr(current, namespace):
    import imp
    m = imp.new_module('DDG4.' + namespace)
    setattr(current, namespace, m)
    ns = m
  evaluator = dd4hep.g4Evaluator()
  cnt = 0
  num = 0
  todo = {}
  strings = {}
  for c in description.constants():
    if c.second.dataType == 'string':
      strings[c.first] = c.second.GetTitle()
    else:
      todo[c.first] = c.second.GetTitle().replace('(int)', '')
  while len(todo) and cnt < 100:
    cnt = cnt + 1
    if cnt == 100:
      logger.error('%s %d out of %d %s "%s": [%s]\n+++ %s',
                   '+++ FAILED to import',
                   len(todo), len(todo) + num,
                   'global values into namespace',
                   ns.__name__, 'Try to continue anyway', 100 * '=')
      for k, v in todo.items():
        if not hasattr(ns, k):
          logger.error('+++ FAILED to import: "' + k + '" = "' + str(v) + '"')
      logger.info('+++ %s', 100 * '=')

    for k, v in list(todo.items()):
      if not hasattr(ns, k):
        val = evaluator.evaluate(v)
        status = evaluator.status()
        if status == 0:
          evaluator.setVariable(k, val)
          setattr(ns, k, val)
          if debug:
            logger.info('Imported global value: "' + k + '" = "' + str(val) + '" into namespace' + ns.__name__)
          del todo[k]
          num = num + 1
  if cnt < 100:
    logger.info('+++ Imported %d global values to namespace:%s', num, ns.__name__,)


def _registerGlobalAction(self, action):
  self.get().registerGlobalAction(Interface.toAction(action))


def _registerGlobalFilter(self, filter):
  self.get().registerGlobalFilter(Interface.toAction(filter))


def _getKernelProperty(self, name):
  ret = Interface.getPropertyKernel(self.get(), name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.get(), name):
    return getattr(self.get(), name)
  elif hasattr(self, name):
    return getattr(self, name)
  msg = 'Geant4Kernel::GetProperty [Unhandled]: Cannot access Kernel.' + name
  raise KeyError(msg)


def _setKernelProperty(self, name, value):
  if Interface.setPropertyKernel(self.get(), str(name), str(value)):
    return
  msg = 'Geant4Kernel::SetProperty [Unhandled]: Cannot set Kernel.' + name + ' = ' + str(value)
  raise KeyError(msg)


def _kernel_phase(self, name):
  return self.addSimplePhase(str(name), False)


def _kernel_worker(self):
  return Kernel(self.get().createWorker())


def _kernel_terminate(self):
  return self.get().terminate()


Kernel.phase = _kernel_phase
Kernel.registerGlobalAction = _registerGlobalAction
Kernel.registerGlobalFilter = _registerGlobalFilter
Kernel.createWorker = _kernel_worker
Kernel.__getattr__ = _getKernelProperty
Kernel.__setattr__ = _setKernelProperty
Kernel.terminate = _kernel_terminate

ActionHandle = Sim.ActionHandle


def SensitiveAction(kernel, nam, det, shared=False):
  return Interface.createSensitive(kernel, str(nam), str(det), shared)


def Action(kernel, nam, shared=False):
  return Interface.createAction(kernel, str(nam), shared)


def Filter(kernel, nam, shared=False):
  return Interface.createFilter(kernel, str(nam), shared)


def PhaseAction(kernel, nam, shared=False):
  return Interface.createPhaseAction(kernel, str(nam), shared)


def RunAction(kernel, nam, shared=False):
  return Interface.createRunAction(kernel, str(nam), shared)


def EventAction(kernel, nam, shared=False):
  return Interface.createEventAction(kernel, str(nam), shared)


def GeneratorAction(kernel, nam, shared=False):
  return Interface.createGeneratorAction(kernel, str(nam), shared)


def TrackingAction(kernel, nam, shared=False):
  return Interface.createTrackingAction(kernel, str(nam), shared)


def SteppingAction(kernel, nam, shared=False):
  return Interface.createSteppingAction(kernel, str(nam), shared)


def StackingAction(kernel, nam, shared=False):
  return Interface.createStackingAction(kernel, str(nam), shared)


def DetectorConstruction(kernel, nam):
  return Interface.createDetectorConstruction(kernel, str(nam))


def PhysicsList(kernel, nam):
  return Interface.createPhysicsList(kernel, str(nam))


def UserInitialization(kernel, nam):
  return Interface.createUserInitialization(kernel, str(nam))


def SensitiveSequence(kernel, nam):
  return Interface.createSensDetSequence(kernel, str(nam))


def _setup(obj):
  def _adopt(self, action):
    self.__adopt(action.get())
  _import_class('Sim', obj)
  o = getattr(current, obj)
  setattr(o, '__adopt', getattr(o, 'adopt'))
  setattr(o, 'adopt', _adopt)
  setattr(o, 'add', _adopt)


def _setup_callback(obj):
  def _adopt(self, action):
    self.__adopt(action.get(), action.callback())
  _import_class('Sim', obj)
  o = getattr(current, obj)
  setattr(o, '__adopt', getattr(o, 'add'))
  setattr(o, 'add', _adopt)


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
_import_class('Sim', 'Geant4Vertex')
_import_class('Sim', 'Geant4Particle')
_import_class('Sim', 'Geant4VertexVector')
_import_class('Sim', 'Geant4ParticleVector')
_import_class('Sim', 'Geant4Action')
_import_class('Sim', 'Geant4Filter')
_import_class('Sim', 'Geant4RunAction')
_import_class('Sim', 'Geant4TrackingAction')
_import_class('Sim', 'Geant4StackingAction')
_import_class('Sim', 'Geant4PhaseAction')
_import_class('Sim', 'Geant4UserParticleHandler')
_import_class('Sim', 'Geant4UserInitialization')
_import_class('Sim', 'Geant4DetectorConstruction')
_import_class('Sim', 'Geant4GeneratorWrapper')
_import_class('Sim', 'Geant4Random')
_import_class('CLHEP', 'HepRandom')
_import_class('CLHEP', 'HepRandomEngine')


def _get(self, name):
  a = Interface.toAction(self)
  ret = Interface.getProperty(a, name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.action, name):
    return getattr(self.action, name)
  elif hasattr(a, name):
    return getattr(a, name)
  msg = 'Geant4Action::GetProperty [Unhandled]: Cannot access property ' + a.name() + '.' + name
  raise KeyError(msg)


def _deUnicode(value):
  """Turn any unicode literal into str, needed when passing to c++.

  Recursively transverses dicts, lists, sets, tuples

  :return: always a str
  """
  if isinstance(value, (bool, float, six.integer_types)):
    value = value
  elif isinstance(value, six.string_types):
    value = str(value)
  elif isinstance(value, (list, set, tuple)):
    value = [_deUnicode(x) for x in value]
  elif isinstance(value, dict):
    tempDict = {}
    for key, val in value.items():
      key = _deUnicode(key)
      val = _deUnicode(val)
      tempDict[key] = val
    value = tempDict
  return str(value)


def _set(self, name, value):
  """This function is called when properties are passed to the c++ objects."""
  a = Interface.toAction(self)
  name = _deUnicode(name)
  value = _deUnicode(value)
  if Interface.setProperty(a, name, value):
    return
  msg = 'Geant4Action::SetProperty [Unhandled]: Cannot set ' + a.name() + '.' + name + ' = ' + value
  raise KeyError(msg)


def _props(obj):
  _import_class('Sim', obj)
  cl = getattr(current, obj)
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


class Geant4:
  """
  Helper object to perform stuff, which occurs very often.
  I am sick of typing the same over and over again.
  Hence, I grouped often used python fragments to this small
  class to re-usage.

  \author  M.Frank
  \version 1.0
  """

  def __init__(self, kernel=None,
               calo='Geant4CalorimeterAction',
               tracker='Geant4SimpleTrackerAction'):
    kernel.UI = "UI"
    kernel.printProperties()
    self._kernel = kernel
    if kernel is None:
      self._kernel = Kernel()
    self.description = self._kernel.detectorDescription()
    self.sensitive_types = {}
    self.sensitive_types['tracker'] = tracker
    self.sensitive_types['calorimeter'] = calo
    self.sensitive_types['escape_counter'] = 'Geant4EscapeCounter'

  def kernel(self):
    """
    Access the worker kernel object.

    \author  M.Frank
    """
    return self._kernel.worker()

  def master(self):
    """
    Access the master kernel object.

    \author  M.Frank
    """
    return self._kernel

  def setupUI(self, typ='csh', vis=False, ui=True, macro=None):
    """
    Configure the Geant4 command executive

    \author  M.Frank
    """
    ui_action = Action(self.master(), "Geant4UIManager/UI")
    if vis:
      ui_action.HaveVIS = True
    else:
      ui_action.HaveVIS = False
    if ui:
      ui_action.HaveUI = True
    else:
      ui_action.HaveUI = False
    ui_action.SessionType = typ
    if macro:
      ui_action.SetupUI = macro
    self.master().registerGlobalAction(ui_action)
    return ui_action

  def setupCshUI(self, typ='csh', vis=False, ui=True, macro=None):
    """
    Configure the Geant4 command executive with a csh like command prompt

    \author  M.Frank
    """
    return self.setupUI(typ='csh', vis=vis, ui=ui, macro=macro)

  def addUserInitialization(self, worker, worker_args=None, master=None, master_args=None):
    """
    Configure Geant4 user initialization for optionasl multi-threading mode

    \author  M.Frank
    """
    init_seq = self.master().userInitialization(True)
    init_action = UserInitialization(self.master(), 'Geant4PythonInitialization/PyG4Init')
    #
    if worker:
      init_action.setWorkerSetup(worker, worker_args)
    else:
      raise RuntimeError('Invalid argument for Geant4 worker initialization')
    #
    if master:
      init_action.setMasterSetup(master, master_args)
    #
    init_seq.adopt(init_action)
    return init_seq, init_action

  def detectorConstruction(self):
    seq = self.master().detectorConstruction(True)
    return seq

  def addDetectorConstruction(self, name_type,
                              field=None, field_args=None,
                              geometry=None, geometry_args=None,
                              sensitives=None, sensitives_args=None,
                              allow_threads=False):
    """
    Configure Geant4 user initialization for optionasl multi-threading mode

    \author  M.Frank
    """
    init_seq = self.master().detectorConstruction(True)
    init_action = DetectorConstruction(self.master(), name_type)
    #
    if geometry:
      init_action.setConstructGeo(geometry, geometry_args)
    #
    if field:
      init_action.setConstructField(field, field_args)
    #
    if sensitives:
      init_action.setConstructSensitives(sensitives, sensitives_args)
    #
    init_seq.adopt(init_action)
    if allow_threads:
      last_action = DetectorConstruction(self.master(), "Geant4PythonDetectorConstructionLast/LastDetectorAction")
      init_seq.adopt(last_action)

    return init_seq, init_action

  def addPhaseAction(self, phase_name, factory_specification, ui=True, instance=None):
    """
    Add a new phase action to an arbitrary step.

    \author  M.Frank
    """
    if instance is None:
      instance = self.kernel()
    action = PhaseAction(instance, factory_specification)
    instance.phase(phase_name).add(action)
    if ui:
      action.enableUI()
    return action

  def addConfig(self, factory_specification):
    """
    Add a new phase action to the 'configure' step.
    Called at the beginning of Geant4Exec::configure.
    The factory specification is the typical string "<factory_name>/<instance name>".
    If no instance name is specified it defaults to the factory name.

    \author  M.Frank
    """
    return self.addPhaseAction('configure', factory_specification, instance=self.master())

  def addInit(self, factory_specification):
    """
    Add a new phase action to the 'initialize' step.
    Called at the beginning of Geant4Exec::initialize.
    The factory specification is the typical string "<factory_name>/<instance name>".
    If no instance name is specified it defaults to the factory name.

    \author  M.Frank
    """
    return self.addPhaseAction('initialize', factory_specification)

  def addStart(self, factory_specification):
    """
    Add a new phase action to the 'start' step.
    Called at the beginning of Geant4Exec::run.
    The factory specification is the typical string "<factory_name>/<instance name>".
    If no instance name is specified it defaults to the factory name.

    \author  M.Frank
    """
    return self.addPhaseAction('start', factory_specification)

  def addStop(self, factory_specification):
    """
    Add a new phase action to the 'stop' step.
    Called at the end of Geant4Exec::run.
    The factory specification is the typical string "<factory_name>/<instance name>".
    If no instance name is specified it defaults to the factory name.

    \author  M.Frank
    """
    return self.addPhaseAction('stop', factory_specification)

  def execute(self, num_events=None):
    """
    Execute the Geant 4 program with all steps.

    \author  M.Frank
    """
    self.kernel().configure()
    self.kernel().initialize()
    if num_events:
      self.kernel().NumEvents = num_events
    self.kernel().run()
    self.kernel().terminate()
    return self

  def printDetectors(self):
    logger.info('+++  List of sensitive detectors:')
    for i in self.description.detectors():
      o = DetElement(i.second.ptr())  # noqa: F405
      sd = self.description.sensitiveDetector(str(o.name()))
      if sd.isValid():
        typ = sd.type()
        sdtyp = 'Unknown'
        if typ in self.sensitive_types:
          sdtyp = self.sensitive_types[typ]
        logger.info('+++  %-32s type:%-12s  --> Sensitive type: %s', o.name(), typ, sdtyp)

  def setupDetector(self, name, action, collections=None):
    # fg: allow the action to be a tuple with parameter dictionary
    sensitive_type = ""
    parameterDict = {}
    if isinstance(action, tuple) or isinstance(action, list):
      sensitive_type = action[0]
      parameterDict = action[1]
    else:
      sensitive_type = action

    seq = SensitiveSequence(self.kernel(), 'Geant4SensDetActionSequence/' + name)
    seq.enableUI()
    acts = []
    if collections is None:
      sd = self.description.sensitiveDetector(str(name))
      ro = sd.readout()
      collections = ro.collectionNames()
      if len(collections) == 0:
        act = SensitiveAction(self.kernel(), sensitive_type + '/' + name + 'Handler', name)
        for parameter, value in six.iteritems(parameterDict):
          setattr(act, parameter, value)
        acts.append(act)

    # Work down the collections if present
    if collections is not None:
      for coll in collections:
        params = {}
        if isinstance(coll, tuple) or isinstance(coll, list):
          if len(coll) > 2:
            coll_nam = str(coll[0])
            sensitive_type = coll[1]
            params = str(coll[2])
          elif len(coll) > 1:
            coll_nam = str(coll[0])
            sensitive_type = coll[1]
          else:
            coll_nam = str(coll[0])
        else:
          coll_nam = str(coll)
        act = SensitiveAction(self.kernel(), sensitive_type + '/' + coll_nam + 'Handler', name)
        act.CollectionName = coll_nam
        for parameter, value in six.iteritems(params):
          setattr(act, parameter, value)
        acts.append(act)

    for act in acts:
      act.enableUI()
      seq.add(act)
    if len(acts) > 1:
      return (seq, acts)
    return (seq, acts[0])

  def setupCalorimeter(self, name, type=None, collections=None):
    self.description.sensitiveDetector(str(name))
    # sd.setType('calorimeter')
    if type is None:
      type = self.sensitive_types['calorimeter']
    return self.setupDetector(name, type, collections)

  def setupTracker(self, name, type=None, collections=None):
    self.description.sensitiveDetector(str(name))
    # sd.setType('tracker')
    if type is None:
      type = self.sensitive_types['tracker']
    return self.setupDetector(name, type, collections)

  def _private_setupField(self, field, stepper, equation, prt):
    import g4units
    field.stepper = stepper
    field.equation = equation
    field.eps_min = 5e-05 * g4units.mm
    field.eps_max = 0.001 * g4units.mm
    field.min_chord_step = 0.01 * g4units.mm
    field.delta_chord = 0.25 * g4units.mm
    field.delta_intersection = 0.001 * g4units.mm
    field.delta_one_step = 0.01 * g4units.mm
    field.largest_step = 1000 * g4units.m
    if prt:
      logger.info('+++++> %s %s %s %s ', field.name, '-> stepper  = ', str(field.stepper), '')
      logger.info('+++++> %s %s %s %s ', field.name, '-> equation = ', str(field.equation), '')
      logger.info('+++++> %s %s %s %s ', field.name, '-> eps_min  = ', str(field.eps_min), '[mm]')
      logger.info('+++++> %s %s %s %s ', field.name, '-> eps_max  = ', str(field.eps_max), '[mm]')
      logger.info('+++++> %s %s %s %s ', field.name, '-> delta_chord        = ', str(field.delta_chord), '[mm]')
      logger.info('+++++> %s %s %s %s ', field.name, '-> min_chord_step     = ', str(field.min_chord_step), '[mm]')
      logger.info('+++++> %s %s %s %s ', field.name, '-> delta_one_step     = ', str(field.delta_one_step), '[mm]')
      logger.info('+++++> %s %s %s %s ', field.name, '-> delta_intersection = ', str(field.delta_intersection), '[mm]')
      logger.info('+++++> %s %s %s %s ', field.name, '-> largest_step       = ', str(field.largest_step), '[mm]')
    return field

  def setupTrackingFieldMT(self, name='MagFieldTrackingSetup',
                           stepper='ClassicalRK4', equation='Mag_UsualEqRhs', prt=False):
    seq, fld = self.addDetectorConstruction("Geant4FieldTrackingConstruction/" + name)
    self._private_setupField(fld, stepper, equation, prt)
    return (seq, fld)

  def setupTrackingField(self, name='MagFieldTrackingSetup',
                         stepper='ClassicalRK4', equation='Mag_UsualEqRhs', prt=False):
    field = self.addConfig('Geant4FieldTrackingSetupAction/' + name)
    self._private_setupField(field, stepper, equation, prt)
    return field

  def setupPhysics(self, name):
    phys = self.master().physicsList()
    phys.extends = name
    phys.decays = True
    phys.enableUI()
    phys.dump()
    return phys

  def addPhysics(self, name):
    phys = self.master().physicsList()
    opt = PhysicsList(self.master(), name)
    opt.enableUI()
    phys.adopt(opt)
    return opt

  def setupGun(self, name, particle, energy, isotrop=True,
               multiplicity=1, position=(0.0, 0.0, 0.0), register=True, **args):
    gun = GeneratorAction(self.kernel(), "Geant4ParticleGun/" + name, True)
    for i in args.items():
      setattr(gun, i[0], i[1])
    gun.energy = energy
    gun.particle = particle
    gun.multiplicity = multiplicity
    gun.position = position
    gun.isotrop = isotrop
    gun.enableUI()
    if register:
      self.kernel().generatorAction().add(gun)
    return gun

  def setupROOTOutput(self, name, output, mc_truth=True):
    """
    Configure ROOT output for the simulated events

    \author  M.Frank
    """
    evt_root = EventAction(self.kernel(), 'Geant4Output2ROOT/' + name, True)
    evt_root.HandleMCTruth = mc_truth
    evt_root.Control = True
    if not output.endswith('.root'):
      output = output + '.root'
    evt_root.Output = output
    evt_root.enableUI()
    self.kernel().eventAction().add(evt_root)
    return evt_root

  def setupLCIOOutput(self, name, output):
    """
    Configure LCIO output for the simulated events

    \author  M.Frank
    """
    evt_lcio = EventAction(self.kernel(), 'Geant4Output2LCIO/' + name, True)
    evt_lcio.Control = True
    evt_lcio.Output = output
    evt_lcio.enableUI()
    self.kernel().eventAction().add(evt_lcio)
    return evt_lcio

  def setupEDM4hepOutput(self, name, output):
    """Configure EDM4hep root output for the simulated events."""
    evt_edm4hep = EventAction(self.kernel(), 'Geant4Output2EDM4hep/' + name, True)
    evt_edm4hep.Control = True
    evt_edm4hep.Output = output
    evt_edm4hep.enableUI()
    self.kernel().eventAction().add(evt_edm4hep)
    return evt_edm4hep

  def buildInputStage(self, generator_input_modules, output_level=None, have_mctruth=True):
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
    ga = self.kernel().generatorAction()
    # Register Generation initialization action
    gen = GeneratorAction(self.kernel(), "Geant4GeneratorActionInit/GenerationInit")
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
    gen = GeneratorAction(self.kernel(), "Geant4InteractionMerger/InteractionMerger")
    gen.enableUI()
    if output_level is not None:
      gen.OutputLevel = output_level
    ga.adopt(gen)

    # Finally generate Geant4 primaries
    if have_mctruth:
      gen = GeneratorAction(self.kernel(), "Geant4PrimaryHandler/PrimaryHandler")
      gen.RejectPDGs = "{1,2,3,4,5,6,21,23,24}"
      gen.enableUI()
      if output_level is not None:
        gen.OutputLevel = output_level
      ga.adopt(gen)
    # Puuuhh! All done.
    return self

  def run(self):
    """
    Execute the main Geant4 action
    \author  M.Frank
    """
    from ROOT import PyDDG4
    PyDDG4.run(self.master().get())
    return self


Simple = Geant4
