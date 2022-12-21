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
from __future__ import absolute_import
import os
import dddigi
import logging
from dd4hep import units
from dddigi import DEBUG, INFO, WARNING, ERROR  # noqa: F401

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

attenuation = {'Minitel1Hits': 50 * units.ns,
               'Minitel2Hits': 50 * units.ns,
               'Minitel3Hits': 50 * units.ns,
}


# ==========================================================================================================
class Test(dddigi.Digitize):

  def __init__(self, geometry=None):
    global attenuation
    dddigi.Digitize.__init__(self, dddigi.Kernel())
    dddigi.setPrintFormat(str('%-32s %5s %s'))
    dddigi.setPrintLevel(INFO)
    self.kernel().printProperties()
    self.geometry = geometry
    self.input = None
    self.main_sequencer()
    self.attenuation = attenuation
    self.used_inputs = []
    self.inputs = ['MiniTel.run00000000.root',
                   'MiniTel.run00000001.root',
                   'MiniTel.run00000002.root',
                   'MiniTel.run00000003.root',
                   'MiniTel.run00000004.root',
                   'MiniTel.run00000005.root',
                   'MiniTel.run00000006.root',
                   'MiniTel.run00000007.root',
                   'MiniTel.run00000008.root']
    if not os.path.exists(self.inputs[0]):
      if os.path.exists('DDDigi'):
        os.chdir('DDDigi')
    if not os.path.exists(self.inputs[0]):
      # This will cause: FileNotFoundError: [Errno 2] No such file or directory: 'xxxxx'
      open(self.inputs[0])

  def segment_action(self, nam, **options):
    obj = dddigi.Interface.createSegmentAction(self.kernel(), str(nam))
    return obj

  def load_geo(self, volume_manager=None):
    fname = "file:" + os.environ['DD4hepExamplesINSTALL'] + "/examples/ClientTests/compact/MiniTelGenerate.xml"
    self.kernel().loadGeometry(str(fname))
    self.printDetectors()
    if volume_manager:
      vm = self.description.volumeManager()
      if not vm.isValid():
        self.description.processXMLString(str("""<plugins>
          <plugin name="DD4hep_VolumeManager"/>
        </plugins>"""))
      self.volumeManager = self.description.volumeManager()
      if self.volumeManager.isValid():
        self.info('+++ Successfully created DD4hep VolumeManager')
    return self

  def data_containers(self):
    return list(self.attenuation.keys())

  def containers(self, count=None):
    if count:
      conts = []
      result = []
      for key in list(self.attenuation.keys()):
        conts.append(key)
        if len(conts) == count:
          result.append(conts)
          conts = []
      if len(conts) > 0:
        result.append(conts)
    else:
      result = list(self.attenuation.keys())
    return result

  def check_creation(self, objs):
    for o in objs:
      if o is None:
        self.error('FAILED  Failed to create object')

  def declare_input(self, name, input, parallel=True):  # noqa: A002
    if not self.input:
      self.input = dddigi.Synchronize(self.kernel(), 'DigiParallelActionSequence/READER')
      self.input.parallel = True

  def next_input(self):
    if len(self.used_inputs) == len(self.inputs):
      self.used_inputs = []
    next_source = self.inputs[len(self.used_inputs)]
    self.used_inputs.append(next_source)
    self.info('Prepariing next input file: ' + str(next_source))
    return next_source

  def run_checked(self, num_events=5, num_threads=5, parallel=3):
    result = "FAILED"
    if self.num_events:
      num_events = int(self.num_events)
    if self.num_threads:
      num_threads = int(self.num_threads)
    if self.events_parallel:
      parallel = int(self.events_parallel)
    evt_done = self.run(num_events=num_events, num_threads=num_threads, parallel=parallel)
    if evt_done == num_events:
        result = "PASSED"
    self.always('%s Test finished after processing %d events. [%d parallel threads, %d parallel events]' \
                % (result, evt_done, num_threads, parallel, ))
    self.always('Test done. Exiting')
    self.kernel().terminate()
    return evt_done

# ==========================================================================================================
def test_setup_1(digi, print_level=WARNING, parallel=True):
  """
      Create default setup for tests. Simply too bad to repeat the same lines over and over again.

      \author  M.Frank
      \version 1.0
  """
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  input = digi.input_action('DigiParallelActionSequence/READER')  # noqa: A001
  input.adopt_action('DigiDDG4ROOT/SignalReader',
                     mask=0xCBAA,
                     input=[digi.next_input()],
                     OutputLevel=print_level, keep_raw=False)
  # ========================================================================================================
  digi.info('Creating collision overlay....')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-1')
  overlay.adopt_action('DigiDDG4ROOT/Read-1',
                       mask=0xCBEE,
                       input=[digi.next_input()],
                       OutputLevel=print_level,
                       keep_raw=False)
  digi.info('Created input.overlay-1')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine',
                               OutputLevel=print_level,
                               parallel=parallel,
                               input_masks=[0xCBAA, 0xCBEE],
                               output_mask=0xAAA0,
                               output_segment='deposits')
  combine.erase_combined = True
  proc = event.adopt_action('DigiContainerSequenceAction/HitP1',
                            parallel=parallel,
                            input_mask=0xAAA0,
                            input_segment='deposits',
                            output_mask=0xEEE5,
                            output_segment='deposits')
  combine = digi.create_action('DigiDepositWeightedPosition/WeightedPosition', OutputLevel=print_level)
  proc.adopt_container_processor(combine, digi.containers())
  conts = [c for c in digi.containers()]
  event.adopt_action('DigiContainerDrop/Drop',
                     containers=conts,
                     input_segment='deposits',
                     input_masks=[0xAAA0])

  return event
