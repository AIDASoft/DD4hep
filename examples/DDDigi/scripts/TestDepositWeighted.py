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

  rdr_output = DigiTest.DEBUG
  input_action = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  signal = input_action.adopt_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()], OutputLevel=rdr_output)
  digi.check_creation([signal])
  # ========================================================================================================
  digi.info('Creating collision overlays....')
  # ========================================================================================================
  overlay = input_action.adopt_action('DigiSequentialActionSequence/Overlay-1')
  evtreader = overlay.adopt_action('DigiDDG4ROOT/Read-1', mask=0x1, input=[digi.next_input()], OutputLevel=rdr_output)
  digi.check_creation([overlay, evtreader])
  digi.info('Created input.overlay-1')
  # ========================================================================================================
  overlay = input_action.adopt_action('DigiSequentialActionSequence/Overlay-2')
  evtreader = overlay.adopt_action('DigiDDG4ROOT/Read-2', mask=0x2, input=[digi.next_input()], OutputLevel=rdr_output)
  digi.check_creation([overlay, evtreader])
  digi.info('Created input.overlay-2')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine',
                               parallel=True,
                               input_masks=[0x0, 0x1, 0x2, 0x3],
                               output_mask=0xFEED,
                               output_segment='deposits',
                               erase_combined=False)
  combine.erase_combined = True
  proc = event.adopt_action('DigiContainerSequenceAction/Weight',
                            parallel=True,
                            input_mask=0xFEED,
                            input_segment='deposits',
                            output_mask=0xBABE,
                            output_segment='deposits')
  count = digi.create_action('DigiDepositWeightedPosition/CellsWeighted')
  proc.adopt_container_processor(count, digi.containers())

  event.adopt_action('DigiStoreDump/StoreDump')
  digi.info('Created event.dump')

  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
