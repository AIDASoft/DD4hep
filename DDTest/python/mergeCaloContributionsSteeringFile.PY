"""ddsim steering file merging the hit contributions of the same MCParticle only for the ECAL barrel of SiD"""
from DDSim.DD4hepSimulation import DD4hepSimulation

SIM = DD4hepSimulation()
SIM.action.mapActions["EcalBarrel"] = ("Geant4ScintillatorCalorimeterAction", {"MergeContributions": True})
