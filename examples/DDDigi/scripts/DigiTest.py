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
from dddigi import DEBUG, INFO, WARNING, ERROR

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

attenuation = {'SiVertexEndcapHits': 50 * units.ns,
               'SiVertexBarrelHits': 50 * units.ns,
               'SiTrackerForwardHits': 50 * units.ns,
               'SiTrackerEndcapHits': 50 * units.ns,
               'SiTrackerBarrelHits': 50 * units.ns,
               'HcalPlugHits': 50 * units.ns,
               'HcalEndcapHits': 50 * units.ns,
               'HcalBarrelHits': 50 * units.ns,
               'EcalBarrelHits': 50 * units.ns,
               'EcalEndcapHits': 50 * units.ns,
               'MuonEndcapHits': 50 * units.ns,
               'MuonBarrelHits': 50 * units.ns,
               'BeamCalHits': 50 * units.ns,
               'LumiCalHits': 50 * units.ns,
}


# ==========================================================================================================
class Test(dddigi.Digitize):

  def __init__(self, geometry=None):
    global attenuation
    dddigi.Digitize.__init__(self, dddigi.Kernel())
    dddigi.setPrintFormat(str('%-32s %5s %s'))
    dddigi.setPrintLevel(dddigi.OutputLevel.INFO)
    self.kernel().printProperties()
    self.geometry = geometry
    self.input = None
    self.main_sequencer()
    self.attenuation = attenuation
    self.used_inputs = []
    self.inputs = ['CLICSiD_2022-11-01_15-54.root',
                   'CLICSiD_2022-11-01_15-10.root',
                   'CLICSiD_2022-10-31_17-26.root',
                   'CLICSiD_2022-10-31_17-55.root',
                   'CLICSiD_2022-10-31_17-55.root',
                   'CLICSiD_2022-10-31_18-20.root',
                   'CLICSiD_2022-10-31_18-40.root',
                   'CLICSiD_2022-10-31_18-59.root',
                   'CLICSiD_2022-10-31_19-18.root',
                   'CLICSiD_2022-10-31_19-36.root',
                   'CLICSiD_2022-10-31_19-53.root',
                   'CLICSiD_2022-10-31_20-11.root']

  def segment_action(self, nam, **options):
    obj = dddigi.Interface.createSegmentAction(self.kernel(), str(nam))
    return obj

  def load_geo(self, volume_manager=None):
    fname = "file:" + os.environ['DD4hepINSTALL'] + "/DDDetectors/compact/SiD.xml"
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

  def declare_input(self, name, input, parallel=True):
    if not self.input:
      self.input = dddigi.Synchronize(self.kernel(), 'DigiParallelActionSequence/READER')
      self.input.parallel = True

  def next_input(self):
    if len(self.used_inputs) == len(self.inputs):
      self.used_inputs = []
    next = self.inputs[len(self.used_inputs)]
    self.used_inputs.append(next)
    self.info('Prepariing next input file: ' + str(next))
    return next

  def run_checked(self, num_events=5, num_threads=5, parallel=3):
    result = "FAILED"
    evt_done = self.run(num_events=num_events, num_threads=num_threads, parallel=parallel)
    if evt_done == num_events:
        result = "PASSED"
    self.always('%s Test finished after processing %d events.' % (result, evt_done,))
    self.always('Test done. Exiting')


# ==========================================================================================================
def test_setup_1(digi, print_level=WARNING, parallel=True):
  """
      Create default setup for tests. Simply too bad to repeat the same lines over and over again.

      \author  M.Frank
      \version 1.0
  """
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  input = digi.input_action('DigiParallelActionSequence/READER')
  input.adopt_action('DigiDDG4ROOT/SignalReader', mask=0xCBAA, input=[digi.next_input()], OutputLevel=print_level, keep_raw=False)
  # ========================================================================================================
  digi.info('Creating collision overlay....')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-1')
  overlay.adopt_action('DigiDDG4ROOT/Read-1', mask=0xCBEE, input=[digi.next_input()], OutputLevel=print_level, keep_raw=False)
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
