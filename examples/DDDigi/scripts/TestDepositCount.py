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

  input = digi.input_action('DigiParallelActionSequence/Reader')
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiSequentialActionSequence/Signal')
  reader = signal.adopt_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()])
  sequence = signal.adopt_action('DigiContainerSequenceAction/Counter',
                                 parallel=True, input_mask=0x0, input_segment='inputs')
  count = digi.create_action('DigiCellMultiplicityCounter/CellCounter')
  sequence.adopt_container_processor(count, digi.containers())
  digi.check_creation([reader, signal, sequence, count])
  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=7, parallel=3)


if __name__ == '__main__':
  run()
