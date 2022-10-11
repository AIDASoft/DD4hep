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
  digi   = DigiTest.Test(geometry=None)
  reader = digi.input_action('DigiROOTInput/SignalReader', mask=0x1, input=['CLICSiD_2022-10-05_13-21.root'])
  dump   = digi.event_action('DigiStoreDump/StoreDump')

  if reader is None: digi.error('FAILED create DigiROOTInput')
  if dump is None: digi.error('FAILED create DigiStoreDump')

  evt_todo = 5
  evt_done = digi.run(num_events=evt_todo, num_threads=5, parallel=3)
  if evt_done == evt_todo: result = "PASSED"
  else: result = "FAILED"
  digi.always('%s Test finished after processing %d events.'%(result, digi.events_done(),))
  digi.always('Test done. Exiting')


if __name__ == '__main__':
  run()
