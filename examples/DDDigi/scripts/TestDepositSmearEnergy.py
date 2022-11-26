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

  event = DigiTest.test_setup_1(digi)
  proc = event.adopt_action('DigiContainerSequenceAction/Smearing',
                            parallel=False,
                            input_mask=0xEEE5,
                            input_segment='deposits',
                            output_mask=0xFFF0,
                            output_segment='outputs')
  smear = digi.create_action('DigiDepositSmearEnergy/Smear')
  #  sigma(E)[GeV]/E[GeV] = 0.02        (systematic) + 0.005 / sqrt(E[GeV]) (intrinsic) + 1keV/E[GeV] (instrumentation)
  #  sigma(E)[GeV]        = 0.02*E[GeV] (systematic) + 0.005 * sqrt(E[GeV]) (intrinsic) + 1keV (instrumentation)
  smear.intrinsic_fluctuation = 0.005 / math.sqrt(units.GeV)
  smear.systematic_resolution = 0.02 / units.GeV
  smear.instrumentation_resolution = 1 * units.keV
  smear.pair_ionisation_energy = 10 * units.eV
  smear.ionization_fluctuation = True
  # proc.adopt_container_processor(smear, conts)
  proc.adopt_container_processor(smear, digi.containers())

  event.adopt_action('DigiStoreDump/HeaderDump')
  # ========================================================================================================
  digi.info('Starting digitization core')
  digi.run_checked(num_events=5, num_threads=7, parallel=5)


if __name__ == '__main__':
  run()
