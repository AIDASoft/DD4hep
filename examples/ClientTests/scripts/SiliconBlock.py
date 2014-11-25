#
#
import os, time, DDG4
from DDG4 import OutputLevel as Output
from SystemOfUnits import *
#
#
"""

   DD4hep simulation example setup using the python configuration

   @author  M.Frank
   @version 1.0

"""
def run():
  kernel = DDG4.Kernel()
  lcdd = kernel.lcdd()
  install_dir = os.environ['DD4hepINSTALL']
  example_dir = install_dir+'/examples/DDG4/examples';
  kernel.loadGeometry("file:"+install_dir+"/examples/ClientTests/compact/SiliconBlock.xml")
  kernel.loadXML("file:"+example_dir+"/DDG4_field.xml")
  DDG4.importConstants(lcdd,debug=False)
  simple = DDG4.Simple(kernel,tracker='Geant4TrackerCombineAction')
  simple.printDetectors()
  # Configure UI
  simple.setupCshUI()

  # Configure Event actions
  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.DEBUG
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  generator_output_level = Output.INFO

  # Configure I/O
  ##evt_lcio = simple.setupLCIOOutput('LcioOutput','SiliconBlock_'+time.strftime('%Y-%m-%d_%H-%M'))
  ##evt_lcio.OutputLevel = generator_output_level
  evt_root = simple.setupROOTOutput('RootOutput','SiliconBlock_'+time.strftime('%Y-%m-%d_%H-%M'))

  """
  Generation of primary particles from LCIO input files: particle file reader
  """
  gen = DDG4.GeneratorAction(kernel,"LCIOInputAction/LCIO1");
  gen.Input = "LCIOFileReader|/afs/cern.ch/user/n/nikiforo/public/Markus/muons.slcio"
  gen.MomentumScale = 1.0
  gen.Mask = 1

  simple.buildInputStage([gen],output_level=generator_output_level)

  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.SaveProcesses = ['Decay']
  part.MinimalKineticEnergy = 100*MeV
  part.OutputLevel = Output.INFO #generator_output_level
  part.enableUI()
  user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = 3.0*m
  user.TrackingVolume_Rmax = 3.0*m
  user.enableUI()
  part.adopt(user)

  simple.setupTracker('SiliconBlockUpper')
  simple.setupTracker('SiliconBlockDown')

  # Now build the physics list:
  phys = simple.setupPhysics('QGSP_BERT')
  ph = DDG4.PhysicsList(kernel,'Geant4PhysicsList/Myphysics')
  ph.addParticleConstructor('G4Geantino')
  ph.addParticleConstructor('G4BosonConstructor')
  ph.enableUI()
  phys.adopt(ph)
  phys.dump()

  kernel.configure()
  kernel.initialize()

  #DDG4.setPrintLevel(Output.DEBUG)
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
