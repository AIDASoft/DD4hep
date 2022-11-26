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
  digi.load_geo()
  input = digi.input_action('DigiParallelActionSequence/READER')
  # ========================================================================
  digi.info('Created SIGNAL input')
  signal = input.adopt_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()])
  digi.check_creation([signal])
  # ========================================================================
  event = digi.event_action('DigiSequentialActionSequence/EventAction')
  seq = event.adopt_action('DigiContainerSequenceAction/ResegmentSeq',
                           parallel=True,
                           input_mask=0x0, input_segment='inputs',
                           output_mask=0xFEED, output_segment='outputs')
  resegment = digi.create_action('DigiResegment/Resegment')
  resegment.detector = 'Minitel1'
  resegment.readout = 'NewMinitel1Hits'
  resegment.descriptor = """
  <readout name="NewMinitel1Hits">
    <segmentation type="CartesianGridXY" grid_size_x="20*mm" grid_size_y="20*mm"/>
    <id>system:6,side:2,module:8,x:28:-12,y:52:-12</id>
  </readout>
  """
  resegment.debug = False
  seq.adopt_container_processor(resegment, 'Minitel1Hits')
  event.adopt_action('DigiStoreDump/StoreDump')

  digi.info('Created event.dump')
  # ========================================================================
  digi.run_checked(num_events=5, num_threads=15, parallel=3)


if __name__ == '__main__':
  run()
