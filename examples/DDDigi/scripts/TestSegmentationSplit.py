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
  digi.load_geo()
  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiROOTInput/SignalReader', mask=0x0, input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine', input_masks=[0x0], deposit_mask=0xFEED)
  combine.erase_combined = False  # Not thread-safe! only do in SequentialActionSequence
  split_action = event.adopt_action('DigiContainerSequenceAction/SplitSequence',
                                    parallel=True,
                                    input_segment='deposits',
                                    mask=0xFEED)
  splitter = digi.create_action('DigiSegmentSplitter/Splitter',
                                parallel=True,
                                split_by='layer',
                                detector='SiTrackerBarrel',
                                processor_type='DigiSegmentDepositPrint')
  split_action.adopt_container_processor(splitter, splitter.collection_names())

  dump = event.adopt_action('DigiStoreDump/StoreDump')
  digi.check_creation([combine, dump, splitter])
  digi.info('Created event.dump')
  # ========================================================================
  digi.run_checked(num_events=1, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
