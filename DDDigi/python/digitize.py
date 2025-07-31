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
import dd4hep
import dddigi


class Digitize(dd4hep.Logger, dd4hep.CommandLine):
  """
   Helper object to perform stuff, which occurs very often.
   I am sick of typing the same over and over again.
   Hence, I grouped often used python fragments to this small
   class to re-usage.

   Long live laziness!


   \author  M.Frank
   \version 1.0

    #import pdb
    #pdb.set_trace()

  """
  def __init__(self, kernel=None):
    dd4hep.Logger.__init__(self, 'dddigi')
    dd4hep.CommandLine.__init__(self)
    self._kernel = kernel
    self._main_processor = None
    self._input_processor = None
    self._event_processor = None
    self._parallel = True
    self._dddigi = dddigi
    self.description = self._kernel.detectorDescription()
    if self.output_level:
      lvl = int(self.output_level)
      self.setPrintLevel(lvl)
      self._kernel.OutputLevel = lvl

  """
     Access the worker kernel object.

     \author  M.Frank
  """

  def kernel(self):
    return self._kernel

  def create_action(self, name, **options):
    action = dddigi.Action(self._kernel, name)
    for option in options.items():
      setattr(action, option[0], option[1])
    return action

  def main_sequencer(self):
    """
    Create main digitization sequencer
    """
    if not self._main_processor:
      self._main_processor = self.create_action('DigiSynchronize/MainDigitizer', parallel=self._parallel)
      self._main_processor.parallel = self._parallel
    return self._main_processor

  def input_action(self, name=None, **options):
    """
    Append a new action to the kernel's main input action sequence
    """
    if not self._input_processor:
      self._input_processor = self._kernel.inputAction()
      self._input_processor.parallel = self._parallel

    if not name:
      return self._input_processor

    act = self.create_action(name, **options)
    self._input_processor.adopt(act)
    return act

  def event_action(self, name=None, register=True, **options):
    """
    Append a new action to the kernel's main event action sequence
    """
    if not self._event_processor:
      self._event_processor = self._kernel.eventAction()
      self._event_processor.parallel = self._parallel

    if not name:
      return self._event_processor

    action = self.create_action(name, **options)
    if register:
      self._event_processor.adopt(action)
    return action

  def output_action(self, name=None, **options):
    """
    Append a new action to the kernel's main output action sequence
    """
    if not self._output_processor:
      self._output_processor = self._kernel.outputAction()
      self._output_processor.parallel = self._parallel

    if not name:
      return self._output_processor

    act = self.create_action(name, **options)
    self._output_processor.adopt(act)
    return act

  def events_done(self):
    """
    Access the number of events which finished processing
    """
    return self.kernel().events_done()

  def events_submitted(self):
    """
    Access the number of submitted events
    """
    return self.kernel().events_submitted()

  def events_processing(self):
    """
    Access the currently number of processing events (events in-flight)
    """
    return self.kernel().events_processing()

  """
     Execute the Geant 4 program with all steps.

     \author  M.Frank
  """
  def execute(self):
    self.kernel().configure()
    self.kernel().initialize()
    self.kernel().run()
    done = self.kernel().events_done()
    self.kernel().terminate()
    return done

  def activeDetectors(self):
    detectors = []
    for i in self.description.detectors():
      o = dd4hep.DetElement(i.second.ptr())  # noqa: F405
      sd = self.description.sensitiveDetector(o.name())
      if sd.isValid():
        d = {'name': o.name(), 'type': sd.type(), 'detector': o, 'sensitive': sd}
        detectors.append(d)
    return detectors

  def printDetectors(self):
    self.info('+++  List of sensitive detectors:')
    dets = self.activeDetectors()
    for d in dets:
      self.always('+++  %-32s ---> type:%-12s' % (d['name'], d['type'],))

  """
     Configure ROOT output for the event digitization

     \author  M.Frank
  """
  def setupROOTOutput(self, name, output, mc_truth=True):
    evt_root = dddigi.EventAction(self.kernel(), 'DigiOutput2ROOT/' + name, True)  # noqa: F405
    evt_root.HandleMCTruth = mc_truth
    evt_root.Control = True
    if not output.endswith('.root'):
      output = output + '.root'
    evt_root.Output = output
    evt_root.enableUI()
    self.kernel().eventAction().add(evt_root)
    return evt_root

  """
     Execute the main Digi action
     \author  M.Frank
  """
  def run(self, num_events, num_threads, parallel):
    krnl = self.kernel()
    krnl.numEvents = num_events
    krnl.numThreads = num_threads   # = number of concurrent threads
    krnl.maxEventsParallel = parallel
    krnl.configure()
    krnl.initialize()
    krnl.run()
    return krnl.events_done()
