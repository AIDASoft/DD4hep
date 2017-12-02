"""

   Subtest using LHeD showing the usage of the G4Particle gun using
   the Geant4GeneratorWrapper object.

   @author  M.Frank
   @version 1.0

"""
def run():
  import logging, LHeD, DDG4
  from DDG4 import OutputLevel as Output
  
  logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)
  lhed = LHeD.LHeD()
  geant4 = lhed.geant4
  kernel = lhed.kernel
  lhed.loadGeometry()
  geant4.printDetectors()
  kernel.UI = "UI"
  geant4.setupCshUI()
  lhed.setupField(quiet=False)
  DDG4.importConstants(kernel.detectorDescription(),debug=False)

  prt = DDG4.EventAction(kernel,'Geant4ParticlePrint/ParticlePrint')
  prt.OutputLevel = Output.INFO
  prt.OutputType  = 3 # Print both: table and tree
  kernel.eventAction().adopt(prt)

  gen = DDG4.GeneratorAction(kernel,"Geant4GeneratorActionInit/GenerationInit")
  kernel.generatorAction().adopt(gen)
  logging.info("#  First particle generator: gun")
  gun = DDG4.GeneratorAction(kernel,"Geant4GeneratorWrapper/Gun");
  gun.Uses     = 'G4ParticleGun'
  gun.Mask     = 1
  kernel.generatorAction().adopt(gun)

  # Merge all existing interaction records
  merger = DDG4.GeneratorAction(kernel,"Geant4InteractionMerger/InteractionMerger")
  merger.enableUI()
  kernel.generatorAction().adopt(merger)


  # And handle the simulation particles.
  part = DDG4.GeneratorAction(kernel,"Geant4ParticleHandler/ParticleHandler")
  kernel.generatorAction().adopt(part)
  part.OutputLevel = Output.INFO
  part.enableUI()
  user = DDG4.Action(kernel,"Geant4TCUserParticleHandler/UserParticleHandler")
  user.TrackingVolume_Zmax = DDG4.EcalEndcap_zmin
  user.TrackingVolume_Rmax = DDG4.EcalBarrel_rmin
  user.enableUI()
  part.adopt(user)

  lhed.setupDetectors()
  lhed.setupPhysics('QGSP_BERT')
  lhed.test_config()
  gun.generator()  # Instantiate gun to be able to set properties from G4 prompt
  kernel.run()
  kernel.terminate()

if __name__ == "__main__":
  run()
