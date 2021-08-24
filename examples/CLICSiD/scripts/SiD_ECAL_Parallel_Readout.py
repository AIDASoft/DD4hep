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
#
#
#
#   Subtest using CLICSid showing the usage of the G4Particle gun using
#   the Geant4GeneratorWrapper object.
#
#   @author  M.Frank
#   @version 1.0
#
# ==========================================================================
from __future__ import absolute_import, unicode_literals
import DDG4
import CLICSid
import logging
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)


def run():
  import os
  import time
  import g4units
  install_dir = os.environ['DD4hepExamplesINSTALL']
  file = str("file:" + install_dir + "/examples/CLICSiD/compact/SiD_ECAL_Parallel_Readout.xml")
  sid = CLICSid.CLICSid()
  sid.kernel.loadGeometry(file)
  sid.kernel.UI = "UI"
  sid.geant4.setupCshUI()
  sid.setupField(quiet=False)
  DDG4.importConstants(sid.kernel.detectorDescription(), debug=False)
  #
  sid.geant4.printDetectors()
  sid.geant4.setupROOTOutput('RootOutput', 'CLICSiD_' + time.strftime('%Y-%m-%d_%H-%M'))
  #
  sid.geant4.setupGun("Gun",
                      Standalone=True,
                      particle='e+',
                      energy=20 * g4units.GeV,
                      position=(0, 0, 0),
                      multiplicity=1,
                      isotrop=True)
  # =============================================================================
  #
  #  Setup the parallel readout for the detector EcalBarrel
  #
  # =============================================================================
  # First use the default definition for the basic setup (and the first sensitive action)
  det = str('EcalBarrel')
  typ = sid.geant4.sensitive_types['calorimeter']
  seq = DDG4.SensitiveSequence(sid.kernel, str('Geant4SensDetActionSequence/') + det)
  seq.enableUI()
  act = DDG4.SensitiveAction(sid.kernel, str(typ + '/EcalBarrelHandler'), det)
  act.enableUI()
  seq.add(act)
  #
  # Add extra parallel readout action with readout EcalBarrelHits_0
  act = DDG4.SensitiveAction(sid.kernel, str(typ + '/EcalBarrelHandler_0'), det)
  act.ReadoutName = 'EcalBarrelHits_0'
  act.enableUI()
  seq.add(act)
  #
  # Add extra parallel readout action with readout EcalBarrelHits_1
  act = DDG4.SensitiveAction(sid.kernel, str(typ + '/EcalBarrelHandler_1'), det)
  act.ReadoutName = 'EcalBarrelHits_1'
  act.enableUI()
  seq.add(act)
  #
  # Add extra parallel readout action with readout EcalBarrelHits_2
  act = DDG4.SensitiveAction(sid.kernel, str(typ + '/EcalBarrelHandler_2'), det)
  act.ReadoutName = 'EcalBarrelHits_2'
  act.enableUI()
  seq.add(act)
  # =============================================================================

  sid.setupPhysics('QGSP_BERT')
  sid.test_config()
  sid.kernel.run()
  sid.kernel.terminate()


if __name__ == "__main__":
  run()
