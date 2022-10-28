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
  input.adopt_action('DigiROOTInput/SignalReader', mask=0x0, input=[digi.next_input()])
  # ========================================================================================================
  digi.info('Creating collision overlays....')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-1')
  overlay.adopt_action('DigiROOTInput/Read-1', mask=0x1, input=[digi.next_input()])
  digi.info('Created input.overlay-1')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-2')
  overlay.adopt_action('DigiROOTInput/Read-2', mask=0x2, input=[digi.next_input()])
  digi.info('Created input.overlay-2')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine',
                               parallel=True,
                               input_masks=[0x0, 0x1, 0x2],
                               output_mask=0xAAA0,
                               output_segment='inputs')
  combine.erase_combined = True
  proc = event.adopt_action('DigiContainerSequenceAction/HitP2',
                            parallel=True,
                            input_mask=0xAAA0,
                            input_segment='inputs',
                            output_mask=0xAAA1,
                            output_segment='inputs')
  count = digi.create_action('DigiDepositWeightedPosition/CellCreator')
  proc.adopt_container_processor(count, digi.containers())
  event.adopt_action('DigiStoreDump/StoreDump')
  # ========================================================================================================
  digi.info('Starting digitization core')
  digi.run_checked(num_events=3, num_threads=25, parallel=5)


if __name__ == '__main__':
  run()
