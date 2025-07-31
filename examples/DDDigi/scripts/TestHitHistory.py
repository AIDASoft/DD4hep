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
  import DigiTest
  digi = DigiTest.Test(geometry=None)

  input_action = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  input_action.adopt_action('DigiDDG4ROOT/SignalReader', mask=0xCBAA, input=[digi.next_input()])
  # ========================================================================================================
  digi.info('Creating collision overlay....')
  # ========================================================================================================
  overlay = input_action.adopt_action('DigiSequentialActionSequence/Overlay-1')
  overlay.adopt_action('DigiDDG4ROOT/Read-1', mask=0xCBEE, input=[digi.next_input()])
  digi.info('Created input.overlay-1')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine',
                               parallel=False,
                               input_masks=[0xCBAA, 0xCBEE],
                               output_mask=0xAAA0,
                               output_segment='deposits')
  combine.erase_combined = False
  proc = event.adopt_action('DigiContainerSequenceAction/HitP2',
                            parallel=False,
                            input_mask=0xAAA0,
                            input_segment='deposits',
                            output_mask=0xEEE5,
                            output_segment='deposits')
  combine = digi.create_action('DigiDepositWeightedPosition/DepoCombine')
  proc.adopt_container_processor(combine, digi.containers())
  conts = [c for c in digi.containers()]
  event.adopt_action('DigiContainerDrop/Drop',
                     containers=conts,
                     input_segment='deposits',
                     input_masks=[0xAAA0])
  event.adopt_action('DigiStoreDump/HeaderDump')
  event.adopt_action('DigiStoreDump/HistoryDump',
                     dump_history=True,
                     containers=['SiTrackerBarrelHits', 'MCParticles'],
                     segments=['deposits'],
                     masks=[0xAAA0, 0xEEE5])
  # ========================================================================================================
  digi.info('Starting digitization core')
  digi.run_checked(num_events=3, num_threads=1, parallel=5)


if __name__ == '__main__':
  run()
