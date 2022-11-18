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
from __future__ import absolute_import, unicode_literals
import logging
#
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)
#
#
"""

   dd4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""


def run():
  import os
  import DDG4
  from DDG4 import OutputLevel as Output
  from g4units import keV

  args = DDG4.CommandLine()
  if args.help:
    import sys
    logger.info("""
         python <dir>/TestUserCommands.py -option [-option]
              -vis                            Enable visualization
              -macro                          Pass G4 macro file to UI executive
    """)
    sys.exit(0)


  kernel = DDG4.Kernel()
  kernel.loadGeometry(str("file:" + os.environ['DD4hepExamplesINSTALL'] + "/examples/ClientTests/compact/BoxTrafos.xml"))
  geant4 = DDG4.Geant4(kernel)
  # Configure UI
  if args.macro:
    ui = geant4.setupCshUI(macro=args.macro, vis=args.vis)
  else:
    ui = geant4.setupCshUI(vis=args.vis)

  ui.HaveUI = False

  ui.ConfigureCommands = [
    '/ddg4/Print/param configure-command-1',
    '/ddg4/Print/print_param',
    '/ddg4/Print/param configure-command-2',
    '/ddg4/Print/print_param'
  ]
  ui.InitializeCommands = [
    '/ddg4/Print/param initialize-command-1',
    '/ddg4/Print/print_param',
    '/ddg4/Print/param initialize-command-2',
    '/ddg4/Print/print_param'
  ]
  ui.PreRunCommands = [
    '/ddg4/Print/param pre-run-command-1',
    '/ddg4/Print/print_param',
    '/ddg4/Print/param pre-run-command-2',
    '/ddg4/Print/print_param'
  ]
  ui.TerminateCommands = [
    '/ddg4/Print/param terminate-command-1',
    '/ddg4/Print/print_param',
    '/ddg4/Print/param terminate-command-2',
    '/ddg4/Print/print_param',
    '/ddg4/UI/terminate']
  # Post-run commands are only executed in interactive mode!
  ui.PostRunCommands = [
    '/ddg4/Print/param post-run-command-1',
    '/ddg4/Print/print_param',
    '/ddg4/Print/param post-run-command-2',
    '/ddg4/Print/print_param'
  ]

  prt = DDG4.Action(kernel, 'TestPrintAction/Print')
  prt.enableUI()
  kernel.registerGlobalAction(prt)

  ui.applyCommand('/ddg4/Print/param interactive-1')
  ui.applyCommand('/ddg4/Print/print_param')

  # Now build the physics list:
  phys = geant4.setupPhysics('QGSP_BERT')
  logger.info('# Execute some G4 action using the UI handle from DDG4....')
  geant4.ui().applyCommand('/ddg4/Print/param interactive-2')
  geant4.ui().applyCommand('/ddg4/Print/print_param')

  # Start the engine...
  logger.info("# No we should see the configuration commands:")
  kernel.configure()
  logger.info("# No we should see the initialization commands:")
  kernel.initialize()
  kernel.NumEvents = 0
  logger.info("# No we should see the pre-run commands:")
  kernel.run()
  logger.info("# No we should see the termination commands:")
  ui.applyCommand('/ddg4/Print/param interactive-3')
  ui.applyCommand('/ddg4/Print/print_param')
  kernel.terminate()


if __name__ == "__main__":
  run()
