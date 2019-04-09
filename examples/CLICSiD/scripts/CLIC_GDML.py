"""

   Perform a material scan using Geant4 shotting geantinos

   @author  M.Frank
   @version 1.0

"""
def run():
  import logging, CLICSid, DDG4
  from DDG4 import OutputLevel as Output
  
  logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
  sid = CLICSid.CLICSid()
  sid.loadGeometry()
  sid.geant4.printDetectors()
  kernel = sid.kernel
  kernel.UI = "UI"
  ui = sid.geant4.setupCshUI(ui=None)
  #
  # Setup the GDML writer action
  writer = DDG4.Action(kernel,'Geant4GDMLWriteAction/Writer')
  writer.enableUI()
  kernel.registerGlobalAction(writer)
  #
  # Now initialize. At the Geant4 command prompt we can write the geometry:
  # Idle> /ddg4/Writer/write
  # or by configuring the UI: 
  ui.Commands = [
    '/ddg4/Writer/Output CLICSiD.gdml',
    '/ddg4/Writer/OverWrite 1',
    '/ddg4/Writer/write'
    ]
  kernel.configure()
  kernel.initialize()
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
