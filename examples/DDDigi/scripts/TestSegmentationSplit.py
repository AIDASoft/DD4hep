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
from g4units import ns

def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  attenuation = digi.attenuation

  input  = digi.input_action('DigiParallelActionSequence/READER')
  #========================================================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiROOTInput/SignalReader', mask=0x0, input = [digi.next_input()])
  #========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine', masks=[0x0, 0x1, 0x2, 0x3], deposit_mask=0xFEED)
  combine.erase_combined = True  # Not thread-safe! only do in SequentialActionSequence
  dump = event.adopt_action('DigiStoreDump/StoreDump')

  splitter = event.adopt_action('DigiSegmentationSplitter/Splitter', 
                                input='deposits',
                                masks=[0xFEED],
                                detector='SiTrackerBarrel');

  digi.info('Created event.dump')
  #========================================================================================================
  evt_todo = 5
  evt_done = digi.run(num_events = evt_todo, num_threads = 5, parallel = 3)
  if evt_done == evt_todo: result = "PASSED"
  else: result = "FAILED"
  digi.always('%s Test finished after processing %d events.'%(result, digi.events_done(),))
  digi.always('Test done. Exiting')

if __name__ == '__main__':
  run()
