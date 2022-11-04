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
  import math
  import DigiTest
  from dd4hep import units
  digi = DigiTest.Test(geometry=None)
  digi.load_geo(volume_manager=True)

  event = DigiTest.test_setup_1(digi)
  # ========================================================================================================
  proc = event.adopt_action('DigiContainerSequenceAction/Smearing',
                            parallel=False,
                            input_mask=0xEEE5,
                            input_segment='deposits')
  smear = digi.create_action('DigiDepositSmearPositionTrack/Smear',
                             deposit_cutoff = 1e-55 * units.MeV,
                             resolution_u=1e-2 * units.mm,
                             resolution_v=1e-2 * units.mm)
  proc.adopt_container_processor(smear, ['SiVertexEndcapHits','SiVertexBarrelHits',
                                         'SiTrackerEndcapHits','SiTrackerBarrelHits',
                                         'SiTrackerForwardHits'])

  event.adopt_action('DigiStoreDump/HeaderDump')
  # ========================================================================================================
  digi.info('Starting digitization core')
  digi.run_checked(num_events=3, num_threads=-1, parallel=5)


if __name__ == '__main__':
  run()
