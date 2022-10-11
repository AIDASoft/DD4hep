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


def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)

  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiROOTInput/SignalReader', mask=0x0, input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine', masks=[0x0, 0x1, 0x2, 0x3], deposit_mask=0xFEED)
  combine.erase_combined = True  # Not thread-safe! only do in SequentialActionSequence
  splitter = event.adopt_action('DigiSegmentationSplitter/Splitter',
                                input='deposits',
                                masks=[0xFEED],
                                detector='SiTrackerBarrel')
  dump = event.adopt_action('DigiStoreDump/StoreDump')
  digi.check_creation([combine, dump, splitter])
  digi.info('Created event.dump')
  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
