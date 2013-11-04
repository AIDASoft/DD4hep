// $Id: Factories.h 797 2013-10-03 19:20:32Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Factories.h"

// Geant4 physics lists
#include "G4DecayPhysics.hh"

// EM physics 
#include "G4EmStandardPhysics_option1.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics_option1)
#include "G4EmStandardPhysics_option2.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics_option2)
#include "G4EmStandardPhysics_option3.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics_option3)
#include "G4EmStandardPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics)
#include "G4EmExtraPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmExtraPhysics)


// Ion and hadrons
#include "G4IonPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonPhysics)
#include "G4QStoppingPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QStoppingPhysics)
#include "G4HadronElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysics)
#include "G4HadronElasticPhysicsLHEP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsLHEP)
#include "G4HadronElasticPhysicsHP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsHP)
#include "G4NeutronTrackingCut.hh"
DECLARE_GEANT4_PHYSICS(G4NeutronTrackingCut)

// LHEP hadrons
#include "HadronPhysicsLHEP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsLHEP)

// QGSP hadrons
#include "HadronPhysicsQGSP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP)
#include "HadronPhysicsQGSP_BERT.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT)
#include "HadronPhysicsQGSP_BERT_HP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_HP)
#include "HadronPhysicsQGSP_BERT_CHIPS.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_CHIPS)
#include "HadronPhysicsQGSP_FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_FTFP_BERT)

// FTFP hadrons
#include "HadronPhysicsFTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsFTFP_BERT)

// Photon Processes:
#include "G4GammaConversion.hh"
DECLARE_GEANT4_PROCESS(G4GammaConversion)
#include "G4PhotoElectricEffect.hh"
DECLARE_GEANT4_PROCESS(G4PhotoElectricEffect)
#include "G4ComptonScattering.hh"
DECLARE_GEANT4_PROCESS(G4ComptonScattering)
#include "G4RayleighScattering.hh"
DECLARE_GEANT4_PROCESS(G4RayleighScattering)
#include "G4GammaConversionToMuons.hh"
DECLARE_GEANT4_PROCESS(G4GammaConversionToMuons)

// Electron/positron processes 
#include "G4eIonisation.hh"
DECLARE_GEANT4_PROCESS(G4eIonisation)
#include "G4eBremsstrahlung.hh"
DECLARE_GEANT4_PROCESS(G4eBremsstrahlung)
#include "G4eMultipleScattering.hh"
DECLARE_GEANT4_PROCESS(G4eMultipleScattering)
#include "G4eplusAnnihilation.hh"
DECLARE_GEANT4_PROCESS(G4eplusAnnihilation)
#include "G4AnnihiToMuPair.hh"
DECLARE_GEANT4_PROCESS(G4AnnihiToMuPair)
#include "G4eeToHadrons.hh"
DECLARE_GEANT4_PROCESS(G4eeToHadrons)

// Muon processes 
#include "G4MuIonisation.hh"
DECLARE_GEANT4_PROCESS(G4MuIonisation)
#include "G4MuBremsstrahlung.hh"
DECLARE_GEANT4_PROCESS(G4MuBremsstrahlung)
#include "G4MuPairProduction.hh"
DECLARE_GEANT4_PROCESS(G4MuPairProduction)
#include "G4MuMultipleScattering.hh"
DECLARE_GEANT4_PROCESS(G4MuMultipleScattering)

// Hadron/ion processes 
#include "G4hIonisation.hh"
DECLARE_GEANT4_PROCESS(G4hIonisation)
#include "G4ionIonisation.hh"
DECLARE_GEANT4_PROCESS(G4ionIonisation)
#include "G4hhIonisation.hh"
DECLARE_GEANT4_PROCESS(G4hhIonisation)
#include "G4mplIonisation.hh"
DECLARE_GEANT4_PROCESS(G4mplIonisation)
#include "G4hMultipleScattering.hh"
DECLARE_GEANT4_PROCESS(G4hMultipleScattering)
#include "G4hBremsstrahlung.hh"
DECLARE_GEANT4_PROCESS(G4hBremsstrahlung)
#include "G4hPairProduction.hh"
DECLARE_GEANT4_PROCESS(G4hPairProduction)

// Coulomb scattering
#include "G4CoulombScattering.hh"
DECLARE_GEANT4_PROCESS(G4CoulombScattering)

// Processes for simulation of polarized electron and gamma beams 
#include "G4PolarizedCompton.hh"
DECLARE_GEANT4_PROCESS(G4PolarizedCompton)
#include "G4PolarizedGammaConversion.hh"
DECLARE_GEANT4_PROCESS(G4PolarizedGammaConversion)
#include "G4PolarizedPhotoElectricEffect.hh"
DECLARE_GEANT4_PROCESS(G4PolarizedPhotoElectricEffect)
#include "G4ePolarizedBremsstrahlung.hh"
DECLARE_GEANT4_PROCESS(G4ePolarizedBremsstrahlung)
#include "G4ePolarizedIonisation.hh"
DECLARE_GEANT4_PROCESS(G4ePolarizedIonisation)
#include "G4eplusPolarizedAnnihilation.hh"
DECLARE_GEANT4_PROCESS(G4eplusPolarizedAnnihilation)

// Processes for simulation of X-rays and optical protons production by charged particles 
#include "G4SynchrotronRadiation.hh"
DECLARE_GEANT4_PROCESS(G4SynchrotronRadiation)
#include "G4TransitionRadiation.hh"
//DECLARE_GEANT4_PROCESS(G4TransitionRadiation) !!virtual abstract!!
#include "G4Cerenkov.hh"
DECLARE_GEANT4_PROCESS(G4Cerenkov)
#include "G4Scintillation.hh"
DECLARE_GEANT4_PROCESS(G4Scintillation)

// Others
#include "G4PAIModel.hh"
DECLARE_GEANT4_PROCESS(G4PAIModel)
#include "G4BraggIonGasModel.hh"
DECLARE_GEANT4_PROCESS(G4BraggIonGasModel)
#include "G4BetheBlochIonGasModel.hh"
DECLARE_GEANT4_PROCESS(G4BetheBlochIonGasModel)
#include "G4WentzelVIModel.hh"
DECLARE_GEANT4_PROCESS(G4WentzelVIModel)
#include "G4UrbanMscModel93.hh"
DECLARE_GEANT4_PROCESS(G4UrbanMscModel93)
#include "G4hImpactIonisation.hh"
DECLARE_GEANT4_PROCESS(G4hImpactIonisation)

//#include "G4InversePEEffect.hh"
//DECLARE_GEANT4_PROCESS(G4InversePEEffect)
//#include "G4hInverseIonisation.hh"
//DECLARE_GEANT4_PROCESS(G4hInverseIonisation)
//#include "G4IonInverseIonisation.hh"
//DECLARE_GEANT4_PROCESS(G4IonInverseIonisation)

#if 0
#include ".hh"
DECLARE_GEANT4_PROCESS()
#endif
