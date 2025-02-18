from DDSim.DD4hepSimulation import DD4hepSimulation
SIM = DD4hepSimulation()

# make ddsim find the sensitive detector for box of straws
SIM.action.calorimeterSDTypes = ['sensitive']
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
