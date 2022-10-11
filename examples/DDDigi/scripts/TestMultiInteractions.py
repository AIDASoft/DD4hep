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
  attenuation = digi.attenuation

  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiROOTInput/SignalReader', mask=0x0, input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================================================
  digi.info('Creating collision overlays....')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-1')
  evtreader = overlay.adopt_action('DigiROOTInput/Reader-1', mask=0x1, input=[digi.next_input()])
  hist_drop = overlay.adopt_action('DigiHitHistoryDrop/Drop-1', masks=[evtreader.mask])
  digi.check_creation([overlay, evtreader, hist_drop])
  digi.info('Created input.overlay25')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-2')
  evtreader = overlay.adopt_action('DigiROOTInput/Reader-2', mask=0x2, input=[digi.next_input()])
  hist_drop = overlay.adopt_action('DigiHitHistoryDrop/Drop-2', masks=[evtreader.mask])
  digi.check_creation([overlay, evtreader, hist_drop])
  digi.info('Created input.overlay50')
  # ========================================================================================================
  overlay = input.adopt_action('DigiSequentialActionSequence/Overlay-3')
  evtreader = overlay.adopt_action('DigiROOTInput/Reader-3', mask=0x3, input=[digi.next_input()])
  hist_drop = overlay.adopt_action('DigiHitHistoryDrop/Drop-3', masks=[evtreader.mask])
  digi.check_creation([overlay, evtreader, hist_drop])
  digi.info('Created input.overlay75')
  # ========================================================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  combine = event.adopt_action('DigiContainerCombine/Combine', masks=[0x0, 0x1, 0x2, 0x3], deposit_mask=0xFEED)
  combine.erase_combined = True  # Not thread-safe! only do in SequentialActionSequence
  dump = event.adopt_action('DigiStoreDump/StoreDump')
  digi.check_creation([combine, dump])
  digi.info('Created event.dump')

  # ========================================================================================================
  digi.run_checked(num_events=5, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
