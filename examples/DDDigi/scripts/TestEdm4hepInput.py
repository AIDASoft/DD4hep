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
  digi = DigiTest.Test(geometry=None, process_data=False)
  read = digi.input_action('DigiEdm4hepInput/SignalReader',
                           mask=0x0,
                           input=['MiniTel_DDG4_edm4hep_data.run00000000.root',
                                  'MiniTel_DDG4_edm4hep_data.run00000001.root',
                                  'MiniTel_DDG4_edm4hep_data.run00000002.root'])
  read.input_section = 'events'
  read.objects_disabled = ['EventHeader']
  read.events_per_file = 5
  digi.event_action('DigiStoreDump/StoreDump', parallel=False)
  digi.run_checked(num_events=100, num_threads=10, parallel=10)


if __name__ == '__main__':
  run()
