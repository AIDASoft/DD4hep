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


def run():
  """
    Small test for process splitting by segmentation.
    Assigned parts of the segmentation are processed by a specified
    container action (here a DigiSegmentDepositPrint instance)

    \author  M.Frank
    \version 1.0
  """
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  digi.load_geo()
  input_action = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================
  digi.info('Created SIGNAL input')
  signal = input_action.adopt_action('DigiDDG4ROOT/SignalReader',
                                     mask=0x0,
                                     input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  split_action = event.adopt_action('DigiContainerSequenceAction/SplitSequence',
                                    parallel=True,
                                    input_mask=0x0,
                                    input_segment='inputs',
                                    output_segment='deposits',
                                    output_mask=0xFEED)
  splitter = digi.create_action('DigiSegmentSplitter/Splitter',
                                parallel=True,
                                split_by='module',
                                detector='Minitel1',
                                processor_type='DigiSegmentDepositPrint')
  split_action.adopt_container_processor(splitter, splitter.collection_names())

  event.adopt_action('DigiStoreDump/StoreDump')
  digi.info('Created event.dump')
  # ========================================================================
  digi.run_checked(num_events=5, num_threads=10, parallel=3)


if __name__ == '__main__':
  run()
