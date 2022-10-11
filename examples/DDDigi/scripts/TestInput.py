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
  reader = digi.input_action('DigiROOTInput/SignalReader', mask=0x1, input=['CLICSiD_2022-10-05_13-21.root'])
  dump = digi.event_action('DigiStoreDump/StoreDump')
  digi.run(num_events = 5, num_threads = 5, parallel = 3)

if __name__ == '__main__':
  run()
