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
import dddigi

def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  #digi.load_geo()
  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiROOTInput/SignalReader', mask=0x0, input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  proc = event.adopt_action('DigiMultiContainerProcessor/ContainerProc',
                            input_masks=[0x0, 0x1, 0x2, 0x3])
  cont = digi.data_containers()
  num = int((len(cont)+2)/3)
  for i in range(num):
    #merge = digi.event_action('DigiSegmentDepositPrint/SegmentPrint_%03d'%(i,), register=None)
    merge = dddigi.Action(digi.kernel(), 'DigiContainerProcessor/SegmentPrint_%03d'%(i,));
    conts = digi.containers(i*3,(i+1)*3)
    proc.adopt_processor(merge, conts)

  #dump = event.adopt_action('DigiStoreDump/StoreDump')
  #digi.check_creation([combine, dump, splitter])
  #digi.info('Created event.dump')
  # ========================================================================
  digi.run_checked(num_events=1, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
