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


# ---------------------------------------------------------------------------
def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  read = digi.input_action('DigiDDG4ROOT/SignalReader', mask=0x0, input=[digi.next_input()])
  dump = digi.event_action('DigiStoreDump/StoreDump', parallel=False)
  writ = digi.output_action('DigiEdm4hepOutput/Writer',
                            parallel=True,
                            input_mask=0x0,
                            input_segment='input',
                            output='MiniTel_DDDigi_edm4hep_data.root')
  proc = digi.create_action('DigiEdm4hepOutputProcessor/edm4hep')
  hit_type = 'TrackerHits'
  if digi.hit_type:
    hit_type = digi.hit_type
  cont = [c + '/' + hit_type for c in digi.containers()]
  writ.adopt_container_processor(proc, cont)
  writ.adopt_container_processor(proc, 'MCParticles/MCParticles')
  digi.check_creation([read, dump])
  digi.run_checked(num_events=10, num_threads=10, parallel=3)


# ---------------------------------------------------------------------------
if __name__ == '__main__':
  run()
