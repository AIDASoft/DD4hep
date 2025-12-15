from DDSim.DD4hepSimulation import DD4hepSimulation
SIM = DD4hepSimulation()

# make ddsim find the sensitive detector for box of straws (type: tracker)
SIM.action.trackerSDTypes = ['sensitive']
SIM.action.tracker = ('Geant4ScintillatorCalorimeterAction', {'UseVolumeManager': False})

SIM.outputFile = "sim.root"
SIM.outputConfig.forceDD4HEP = True

# Trigger printout of event seed
SIM.random.enableEventSeed = True

# Fix random seed for all events (debugging only):
# SIM.random.seed = <number>

SIM.filter.calo = ""

# Configure the regexSD for the BoxOfStraws gas_
SIM.geometry.regexSensitiveDetector['BoxOfStrawsDet'] = {'Match': ['gas_'],
                                                         'OutputLevel': 4,
                                                         }


# Disable userParticleHander for box of straws
SIM.part.userParticleHandler = ""
