"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
from __future__ import absolute_import, unicode_literals
from g4units import *


def run():
  import CLICSid
  import DDG4
  from DDG4 import OutputLevel as Output

  sid = CLICSid.CLICSid()
  sid.loadGeometry()
  sid.geant4.printDetectors()
  kernel = sid.kernel
  kernel.UI = "UI"
  ui = sid.geant4.setupCshUI(ui=None)
  #
  # Setup the GDML writer action
  writer = DDG4.Action(kernel, 'Geant4GDMLWriteAction/Writer')
  writer.enableUI()
  kernel.registerGlobalAction(writer)
  sid.setupPhysics('QGSP_BERT')
  #
  gen = sid.geant4.setupGun('Gun', 'pi-', 10 * GeV, Standalone=True)
  # Now initialize. At the Geant4 command prompt we can write the geometry:
  # Idle> /ddg4/Writer/write
  # or by configuring the UI using ui.Commands
  #
  # Please note: The Geant4 physics list must be initialized BEFORE
  # invoking the writer with options. Otherwise the particle definitions
  # are missing!
  # If you ONLY want to dump the geometry to GDML you must call
  # /run/beamOn 0
  # before writing the GDML file!
  # You also need to setup a minimal generation action like:
  # sid.geant4.setupGun('Gun','pi-',10*GeV,Standalone=True)
  #
  ui.Commands = [
      '/run/beamOn 0',
      '/ddg4/Writer/Output CLICSiD.gdml',
      '/ddg4/Writer/OverWrite 1',
      '/ddg4/Writer/write'
  ]
  kernel.NumEvents = 0
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()


if __name__ == "__main__":
  run()
