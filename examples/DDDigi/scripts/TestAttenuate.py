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
from g4units import ns


def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  attenuation = digi.attenuation

  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================================================
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover-25')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader-25ns', mask=0x1, input=[digi.next_input()], keep_raw=False)
  attenuate = spillover.adopt_action('DigiAttenuatorSequence/Att-25ns',
                                     t0=-25 * ns,
                                     signal_decay='exponential',
                                     processor_type='DigiAttenuator',
                                     input_mask=evtreader.mask,
                                     input_segment='inputs',
                                     containers=attenuation)
  hist_drop = spillover.adopt_action('DigiHitHistoryDrop/Drop-25ns', masks=[evtreader.mask])
  digi.check_creation([spillover, evtreader, attenuate, hist_drop])
  digi.info('Created input.spillover-25')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine',
                               input_masks=[0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6],
                               input_segment='inputs',
                               output_mask=0xFEED,
                               output_segment='deposits')
  evtdump = event.adopt_action('DigiStoreDump/StoreDump')
  digi.check_creation([evtdump])
  digi.info('Created event.dump')
  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
