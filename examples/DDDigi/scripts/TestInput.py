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
  digi = DigiTest.Test(geometry=None)
  read = digi.input_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()])
  dump = digi.event_action('DigiStoreDump/StoreDump', parallel=False)
  digi.check_creation([read, dump])
  digi.run_checked(num_events=5, num_threads=5, parallel=3)


if __name__ == '__main__':
  run()
