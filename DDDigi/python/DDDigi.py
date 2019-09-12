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

logger = logging.getLogger(__name__)


def loadDDDigi():
  import ROOT
  from ROOT import gSystem

  # Try to load libglapi to avoid issues with TLS Static
  # Turn off all errors from ROOT about the library missing
  orgLevel = ROOT.gErrorIgnoreLevel
  ROOT.gErrorIgnoreLevel = 6000
  gSystem.Load("libglapi")
  ROOT.gErrorIgnoreLevel = orgLevel

  import platform
  import os
  if platform.system() == "Darwin":
    gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])

  result = gSystem.Load("libDDDigiPlugins")
  if result < 0:
    raise Exception('DDDigi.py: Failed to load the DDDigi library libDDDigiPlugins: ' + gSystem.GetErrorStr())
  from ROOT import dd4hep as module
  return module


# We are nearly there ....
current = __import__(__name__)


def _import_class(ns, nam):
  scope = getattr(current, ns)
  setattr(current, nam, getattr(scope, nam))


# ---------------------------------------------------------------------------
#
try:
  dd4hep = loadDDDigi()
except Exception as X:
  logger.error('+--%-100s--+', 100 * '-')
  logger.error('|  %-100s  |', 'Failed to load DDDigi library:')
  logger.error('|  %-100s  |', str(X))
  logger.error('+--%-100s--+', 100 * '-')
  exit(1)

core = dd4hep
digi = dd4hep.digi
Kernel = digi.KernelHandle
Interface = digi.DigiActionCreation
Detector = core.Detector
#
#
# ---------------------------------------------------------------------------


def _constant(self, name):
  return self.constantAsString(name)


Detector.globalVal = _constant
# ---------------------------------------------------------------------------

"""
  Import the Detector constants into the DDDigi namespace
"""


def importConstants(description, namespace=None, debug=False):
  ns = current
  if namespace is not None and not hasattr(current, namespace):
    import imp
    m = imp.new_module('DDDigi.' + namespace)
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
      logger.info('%s %d out of %d %s "%s": [%s]\n+++ %s',
                  '+++ FAILED to import',
                  len(todo), len(todo) + num,
                  'global values into namespace',
                  ns.__name__, 'Try to continue anyway', 100 * '=')
      for k, v in todo.items():
        if not hasattr(ns, k):
          logger.info('+++ FAILED to import: "' + k + '" = "' + str(v) + '"')
      logger.info('+++ %s', 100 * '=')

    for k, v in todo.items():
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

# ---------------------------------------------------------------------------


def _getKernelProperty(self, name):
  ret = Interface.getPropertyKernel(self.get(), name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.get(), name):
    return getattr(self.get(), name)
  elif hasattr(self, name):
    return getattr(self, name)
  msg = 'DigiKernel::GetProperty [Unhandled]: Cannot access Kernel.' + name
  raise KeyError(msg)

# ---------------------------------------------------------------------------


def _setKernelProperty(self, name, value):
  if Interface.setPropertyKernel(self.get(), str(name), str(value)):
    return
  msg = 'DigiKernel::SetProperty [Unhandled]: Cannot set Kernel.' + name + ' = ' + str(value)
  raise KeyError(msg)

# ---------------------------------------------------------------------------


def _kernel_terminate(self):
  return self.get().terminate()


# ---------------------------------------------------------------------------
Kernel.__getattr__ = _getKernelProperty
Kernel.__setattr__ = _setKernelProperty
Kernel.terminate = _kernel_terminate
# ---------------------------------------------------------------------------
ActionHandle = digi.ActionHandle
# ---------------------------------------------------------------------------


def Action(kernel, nam, parallel=False):
  obj = Interface.createAction(kernel, str(nam))
  obj.parallel = parallel
  return obj
# ---------------------------------------------------------------------------


def TestAction(kernel, nam, sleep=0):
  obj = Interface.createAction(kernel, str('DigiTestAction/' + nam))
  if sleep != 0:
    obj.sleep = sleep
  return obj
# ---------------------------------------------------------------------------


def ActionSequence(kernel, nam, parallel=False):
  obj = Interface.createSequence(kernel, str(nam))
  obj.parallel = parallel
  return obj
# ---------------------------------------------------------------------------


def Synchronize(kernel, nam, parallel=False):
  obj = Interface.createSync(kernel, str(nam))
  obj.parallel = parallel
  return obj
# ---------------------------------------------------------------------------


def _setup(obj):
  def _adopt(self, action):
    self.__adopt(action.get())
  _import_class('digi', obj)
  o = getattr(current, obj)
  setattr(o, '__adopt', getattr(o, 'adopt'))
  setattr(o, 'adopt', _adopt)
  # setattr(o,'add',_adopt)


# ---------------------------------------------------------------------------
_setup('DigiActionSequence')
_setup('DigiSynchronize')
_import_class('digi', 'DigiKernel')
_import_class('digi', 'DigiContext')
_import_class('digi', 'DigiAction')

# ---------------------------------------------------------------------------


def _get(self, name):
  # import traceback
  a = Interface.toAction(self)
  ret = Interface.getProperty(a, name)
  if ret.status > 0:
    return ret.data
  elif hasattr(self.action, name):
    return getattr(self.action, name)
  elif hasattr(a, name):
    return getattr(a, name)
  # elif hasattr(self,name):
  #  return getattr(self,name)
  # traceback.print_stack()
  msg = 'DigiAction::GetProperty [Unhandled]: Cannot access property ' + a.name() + '.' + name
  raise KeyError(msg)


def _set(self, name, value):
  a = Interface.toAction(self)
  if Interface.setProperty(a, name, str(value)):
    return
  msg = 'DigiAction::SetProperty [Unhandled]: Cannot set ' + a.name() + '.' + name + ' = ' + str(value)
  raise KeyError(msg)


def _props(obj):
  _import_class('digi', obj)
  cl = getattr(current, obj)
  cl.__getattr__ = _get
  cl.__setattr__ = _set


_props('ActionHandle')
_props('ActionSequenceHandle')
_props('SynchronizeHandle')

"""
   Helper object to perform stuff, which occurs very often.
   I am sick of typing the same over and over again.
   Hence, I grouped often used python fragments to this small
   class to re-usage.

   Long live laziness!


   \author  M.Frank
   \version 1.0

"""


class Digitize:
  def __init__(self, kernel=None):
    kernel.printProperties()
    self._kernel = kernel
    if kernel is None:
      self._kernel = Kernel()
    self.description = self._kernel.detectorDescription()

  """
     Access the worker kernel object.

     \author  M.Frank
  """

  def kernel(self):
    return self._kernel

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

  def activeDetectors(self):
    detectors = []
    for i in self.description.detectors():
      o = DetElement(i.second.ptr())  # noqa: F405
      sd = self.description.sensitiveDetector(o.name())
      if sd.isValid():
        d = {'name': o.name(), 'type': sd.type(), 'detector': o, 'sensitive': sd}
        detectors.append(d)
    return detectors

  def printDetectors(self):
    logger.info('+++  List of sensitive detectors:')
    dets = self.activeDetectors()
    for d in dets:
      logger.info('+++  %-32s ---> type:%-12s', d['name'], d['type'])

  def setupDetector(self, name, collections=None, modules=None):
    seq = ActionSequence(self.kernel(), 'DigiActionSequence/' + name)
    actions = []
    if isinstance(modules, tuple) or isinstance(modules, list):
      for m in modules:
        if isinstance(m, str):
          a = Action(self.kernel(), m)
          actions.append(a)
        elif isinstance(m, tuple) or isinstance(m, list):
          a = Action(self.kernel(), m[0])
          actions.append(a)
          if len(m) > 1:
            params = m[1]
            for k, v in params.items():
              setattr(a, k, v)
        else:
          actions.append(m)
    for a in actions:
      seq.adopt(a)
    return (seq, actions)

  """
     Configure ROOT output for the event digitization

     \author  M.Frank
  """

  def setupROOTOutput(self, name, output, mc_truth=True):
    evt_root = EventAction(self.kernel(), 'DigiOutput2ROOT/' + name, True)  # noqa: F405
    evt_root.HandleMCTruth = mc_truth
    evt_root.Control = True
    if not output.endswith('.root'):
      output = output + '.root'
    evt_root.Output = output
    evt_root.enableUI()
    self.kernel().eventAction().add(evt_root)
    return evt_root

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
    gen = GeneratorAction(self.kernel(), "DigiGeneratorActionInit/GenerationInit")  # noqa: F405
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
    gen = GeneratorAction(self.kernel(), "DigiInteractionMerger/InteractionMerger")  # noqa: F405
    gen.enableUI()
    if output_level is not None:
      gen.OutputLevel = output_level
    ga.adopt(gen)

    # Finally generate Digi primaries
    if have_mctruth:
      gen = GeneratorAction(self.kernel(), "DigiPrimaryHandler/PrimaryHandler")  # noqa: F405
      gen.RejectPDGs = "{1,2,3,4,5,6,21,23,24}"
      gen.enableUI()
      if output_level is not None:
        gen.OutputLevel = output_level
      ga.adopt(gen)
    # Puuuhh! All done.
    return self

  """
     Execute the main Digi action
     \author  M.Frank
  """

  def run(self):
    # self.kernel().configure()
    # self.kernel().initialize()
    # self.kernel().run()
    # self.kernel().terminate()
    from ROOT import PyDDDigi
    PyDDDigi.run(self.kernel().get())
    return self
