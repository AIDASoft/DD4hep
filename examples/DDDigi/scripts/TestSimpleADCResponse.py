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

  # ========================================================================================================
  input = digi.input_action('DigiSequentialActionSequence/READER')
  input.adopt_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()])
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  event.adopt_action('DigiStoreDump/DumpInput')
  proc = event.adopt_action('DigiContainerCombine/Combine',
                            parallel=True,
                            input_masks=[0x0],
                            input_segment='inputs',
                            output_mask=0xFEED,
                            output_segment='deposits',
                            erase_combined=False)
  event.adopt_action('DigiStoreDump/DumpCombine')
  proc = event.adopt_action('DigiContainerSequenceAction/ADCsequence',
                            parallel=True,
                            input_mask=0xFEED,
                            input_segment='deposits',
                            output_mask=0xBABE,
                            output_segment='output')
  count = digi.create_action('DigiSimpleADCResponse/ADCCreate')
  proc.adopt_container_processor(count, digi.containers())
  event.adopt_action('DigiContainerDrop/DropCombine',
                     parallel=True,
                     input_masks=[0xFEED],
                     input_segment='deposits')
  event.adopt_action('DigiStoreDump/DumpOutput')
  digi.info('Created event.dump')

  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=1, parallel=3)


if __name__ == '__main__':
  run()
