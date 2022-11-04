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
  resegment.detector = 'EcalBarrel'
  resegment.readout = 'NewEcalBarrelHits'
  resegment.descriptor = """
  <readout name="NewEcalBarrelHits">
    <segmentation type="CartesianGridXY" grid_size_x="10" grid_size_y="10"/>
    <id>system:8,barrel:3,module:4,layer:6,slice:5,x:32:-16,y:-16</id>
  </readout>
  """
  resegment.debug = False
  seq.adopt_container_processor(resegment, 'EcalBarrelHits')
  event.adopt_action('DigiStoreDump/StoreDump')

  digi.info('Created event.dump')
  # ========================================================================
  digi.run_checked(num_events=3, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
