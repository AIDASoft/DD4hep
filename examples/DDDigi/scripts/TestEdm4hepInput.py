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
                           input=['MiniTel_edm4hep_DDG4_data.run00000000.root'])
  read.input_section = 'events'
  read.objects_disabled = ['EventHeader']
  digi.event_action('DigiStoreDump/StoreDump', parallel=False)
  digi.run_checked(num_events=5, num_threads=1, parallel=1)


if __name__ == '__main__':
  run()
