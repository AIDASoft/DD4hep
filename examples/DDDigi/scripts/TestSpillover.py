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
  rdr_output = DigiTest.DEBUG
  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================================================
  input.adopt_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()], OutputLevel=rdr_output)
  digi.info('Created input.signal')
  # ========================================================================================================
  digi.info('Creating spillover sequence for EARLIER bunch crossings.....')
  # ========================================================================================================
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover-25')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader-25ns', mask=0x1, input=[digi.next_input()], OutputLevel=rdr_output)
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
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover-50')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader-50ns', mask=0x2, input=[digi.next_input()], OutputLevel=rdr_output)
  attenuate = spillover.adopt_action('DigiAttenuatorSequence/Att-50ns',
                                     t0=-50 * ns, input_mask=evtreader.mask, containers=attenuation, OutputLevel=rdr_output)
  hist_drop = spillover.adopt_action('DigiHitHistoryDrop/Drop-50ns', masks=[evtreader.mask])
  digi.check_creation([spillover, evtreader, attenuate, hist_drop])
  digi.info('Created input.spillover-50')
  # ========================================================================================================
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover-75')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader-75ns', mask=0x3, input=[digi.next_input()], OutputLevel=rdr_output)
  attenuate = spillover.adopt_action('DigiAttenuatorSequence/Att-75ns',
                                     t0=-75 * ns, input_mask=evtreader.mask, containers=attenuation, OutputLevel=rdr_output)
  hist_drop = spillover.adopt_action('DigiHitHistoryDrop/Drop-75ns', masks=[evtreader.mask])
  digi.check_creation([spillover, evtreader, attenuate, hist_drop])
  digi.info('Created input.spillover-75')
  # ========================================================================================================
  digi.info('Creating spillover sequence for LATER bunch crossings.....')
  # ========================================================================================================
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover+25')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader+25ns', mask=0x4, input=[digi.next_input()], OutputLevel=rdr_output)
  attenuate = spillover.adopt_action('DigiAttenuatorSequence/Att+25ns',
                                     t0=25 * ns, input_mask=evtreader.mask, containers=attenuation, OutputLevel=rdr_output)
  hist_drop = spillover.adopt_action('DigiHitHistoryDrop/Drop+25ns', masks=[evtreader.mask])
  digi.check_creation([spillover, evtreader, attenuate, hist_drop])
  digi.info('Created input.spillover+25')
  # ========================================================================================================
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover+50')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader+50ns', mask=0x5, input=[digi.next_input()], OutputLevel=rdr_output)
  attenuate = spillover.adopt_action('DigiAttenuatorSequence/Att+50ns',
                                     t0=50 * ns, input_mask=evtreader.mask, containers=attenuation, OutputLevel=rdr_output)
  hist_drop = spillover.adopt_action('DigiHitHistoryDrop/Drop_50ns', masks=[evtreader.mask])
  digi.check_creation([spillover, evtreader, attenuate, hist_drop])
  digi.info('Created input.spillover+50')
  # ========================================================================================================
  spillover = input.adopt_action('DigiSequentialActionSequence/Spillover+75')
  evtreader = spillover.adopt_action('DigiDDG4ROOT/Reader+75ns', mask=0x6, input=[digi.next_input()], OutputLevel=rdr_output)
  attenuate = spillover.adopt_action('DigiAttenuatorSequence/Att+75ns',
                                     t0=75 * ns, input_mask=evtreader.mask, containers=attenuation, OutputLevel=rdr_output)
  hist_drop = spillover.adopt_action('DigiHitHistoryDrop/Drop+75ns', masks=[evtreader.mask])
  digi.check_creation([spillover, evtreader, attenuate, hist_drop])
  digi.info('Created input.spillover+75')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine',
                               input_masks=[0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6],
                               input_segment='inputs',
                               output_mask=0xFEED,
                               output_segment='deposits')
  combine.erase_combined = True  # Not thread-safe! only do in SequentialActionSequence
  evtdump = event.adopt_action('DigiStoreDump/StoreDump')
  digi.check_creation([combine, evtdump])
  digi.info('Created event.dump')
  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
