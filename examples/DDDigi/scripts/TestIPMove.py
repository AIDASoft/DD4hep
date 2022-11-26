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
  signal = input.adopt_action('DigiSequentialActionSequence/Signal')
  signal.adopt_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()])
  set_ip = signal.adopt_action('DigiIPCreate/SignalIP')
  set_ip.offset_ip = [1, 2, 3]
  set_ip.sigma_ip = [.5, .5, 3.0]
  move_seq = signal.adopt_action('DigiContainerSequenceAction/MoveSignal',
                                 parallel=True, input_mask=0x0, input_segment='inputs')
  mover = digi.create_action('DigiIPMover/MoveIPSignal')
  mover.adopt_property(set_ip, "interaction_point", "interaction_point")
  conts = [c for c in digi.containers()]
  conts.append('MCParticles')
  move_seq.adopt_container_processor(mover, conts)
  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=7, parallel=3)


if __name__ == '__main__':
  run()
