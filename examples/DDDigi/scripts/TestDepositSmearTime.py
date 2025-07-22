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


def run():
  import DigiTest
  from dd4hep import units
  digi = DigiTest.Test(geometry=None)
  digi.kernel().OutputLevel = DigiTest.INFO
  digi.load_geo(volume_manager=True)

  event = DigiTest.test_setup_1(digi)
  # ========================================================================================================
  proc = event.adopt_action('DigiContainerSequenceAction/Smearing',
                            parallel=False,
                            input_mask=0xEEE5,
                            input_segment='deposits')
  smear = digi.create_action('DigiDepositSmearTime/Smear',
                             resolution_time=1e-6 * units.second)
  proc.adopt_container_processor(smear, digi.containers())
  event.adopt_action('DigiStoreDump/HeaderDump', OutputLevel=DigiTest.INFO)
  # ========================================================================================================
  digi.info('Starting digitization core')
  digi.run_checked(num_events=5, num_threads=10, parallel=5)


if __name__ == '__main__':
  run()
