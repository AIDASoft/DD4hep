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
import dddigi


def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  # digi.load_geo()
  input_action = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================
  digi.info('Created SIGNAL input')
  signal = input_action.adopt_action('DigiDDG4ROOT/SignalReader',
                                     mask=0xFEED,
                                     input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  proc = event.adopt_action('DigiMultiContainerProcessor/ContainerProc',
                            parallel=True, input_masks=[0x0, 0x1, 0x2, 0x3, 0xFEED])
  conts = digi.containers(2)
  for i in range(len(conts)):
    merge = dddigi.Action(digi.kernel(), 'DigiContainerProcessor/SegmentPrint_%03d' % (i,))
    proc.adopt_processor(merge, conts[i])
  # ========================================================================
  digi.run_checked(num_events=5, num_threads=15, parallel=3)


if __name__ == '__main__':
  run()
