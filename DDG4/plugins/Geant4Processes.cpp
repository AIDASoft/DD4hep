// $Id:$
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
#include "G4Version.hh"

#if G4VERSION_NUMBER>=960
#define GEANT4_9_6
#endif

// ======================================================================
// Predefined physics processes to be attached to particles
//
//  Usage:
//
//  <physicslist name="Geant4PhysicsList/MyPhysics.0">
//    <processes>
//      <particle name="e[+-]" cut="1*mm">
//        <process name="G4eMultipleScattering" ordAtRestDoIt="-1" ordAlongSteptDoIt="1" ordPostStepDoIt="1"/>
//        <process name="G4eIonisation"         ordAtRestDoIt="-1" ordAlongSteptDoIt="2" ordPostStepDoIt="2"/>
//      </particle>
//    </processes>
//  </physicslist>
//
//
// ======================================================================

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

#if G4VERSION_NUMBER < 1000
#include "G4UrbanMscModel93.hh"
DECLARE_GEANT4_PROCESS(G4UrbanMscModel93)
#endif

#include "G4hImpactIonisation.hh"
DECLARE_GEANT4_PROCESS(G4hImpactIonisation)

//#include "G4InversePEEffect.hh"
//DECLARE_GEANT4_PROCESS(G4InversePEEffect)
//#include "G4hInverseIonisation.hh"
//DECLARE_GEANT4_PROCESS(G4hInverseIonisation)
//#include "G4IonInverseIonisation.hh"
//DECLARE_GEANT4_PROCESS(G4IonInverseIonisation)

// ==================================================================================
#if G4VERSION_NUMBER>=960 and G4VERSION_NUMBER<1000
#include "G4UrbanMscModel93.hh"
DECLARE_GEANT4_PROCESS(G4UrbanMscModel93)
// ==================================================================================
#elif G4VERSION_NUMBER>=1000

// Stuff commented cannot be instantiated!!

#include "G4AdjointBremsstrahlungModel.hh"
DECLARE_GEANT4_PROCESS(G4AdjointBremsstrahlungModel)
#include "G4AdjointComptonModel.hh"
DECLARE_GEANT4_PROCESS(G4AdjointComptonModel)
#include "G4AdjointeIonisationModel.hh"
DECLARE_GEANT4_PROCESS(G4AdjointeIonisationModel)
#include "G4AdjointIonIonisationModel.hh"
DECLARE_GEANT4_PROCESS(G4AdjointIonIonisationModel)
#include "G4AdjointPhotoElectricModel.hh"
DECLARE_GEANT4_PROCESS(G4AdjointPhotoElectricModel)
#include "G4BetheBlochModel.hh"
DECLARE_GEANT4_PROCESS(G4BetheBlochModel)
#include "G4BetheBlochNoDeltaModel.hh"
DECLARE_GEANT4_PROCESS(G4BetheBlochNoDeltaModel)
#include "G4BetheHeitlerModel.hh"
DECLARE_GEANT4_PROCESS(G4BetheHeitlerModel)
#include "G4BoldyshevTripletModel.hh"
DECLARE_GEANT4_PROCESS(G4BoldyshevTripletModel)
#include "G4BraggIonModel.hh"
DECLARE_GEANT4_PROCESS(G4BraggIonModel)
#include "G4BraggModel.hh"
DECLARE_GEANT4_PROCESS(G4BraggModel)
#include "G4BraggNoDeltaModel.hh"
DECLARE_GEANT4_PROCESS(G4BraggNoDeltaModel)
#include "G4ChipsElasticModel.hh"
DECLARE_GEANT4_PROCESS(G4ChipsElasticModel)
#include "G4DNABornExcitationModel.hh"
DECLARE_GEANT4_PROCESS(G4DNABornExcitationModel)
#include "G4eBremParametrizedModel.hh"
DECLARE_GEANT4_PROCESS(G4eBremParametrizedModel)
#include "G4eBremsstrahlungRelModel.hh"
DECLARE_GEANT4_PROCESS(G4eBremsstrahlungRelModel)
#include "G4ecpssrBaseKxsModel.hh"
DECLARE_GEANT4_PROCESS(G4ecpssrBaseKxsModel)
#include "G4ecpssrBaseLixsModel.hh"
DECLARE_GEANT4_PROCESS(G4ecpssrBaseLixsModel)
#include "G4ecpssrFormFactorKxsModel.hh"
DECLARE_GEANT4_PROCESS(G4ecpssrFormFactorKxsModel)
//#include "G4ee2KChargedModel.hh"
//DECLARE_GEANT4_PROCESS(G4ee2KChargedModel)
//#include "G4ee2KNeutralModel.hh"
//DECLARE_GEANT4_PROCESS(G4ee2KNeutralModel)
//#include "G4eeTo3PiModel.hh"
//DECLARE_GEANT4_PROCESS(G4eeTo3PiModel)
//#include "G4eeToHadronsModel.hh"
//DECLARE_GEANT4_PROCESS(G4eeToHadronsModel)
#include "G4ecpssrFormFactorLixsModel.hh"
DECLARE_GEANT4_PROCESS(G4ecpssrFormFactorLixsModel)
#include "G4ecpssrFormFactorMixsModel.hh"
DECLARE_GEANT4_PROCESS(G4ecpssrFormFactorMixsModel)
#include "G4EmMultiModel.hh"
DECLARE_GEANT4_PROCESS(G4EmMultiModel)
#include "G4ePolarizedBremsstrahlungModel.hh"
DECLARE_GEANT4_PROCESS(G4ePolarizedBremsstrahlungModel)
#include "G4FTFModel.hh"
DECLARE_GEANT4_PROCESS(G4FTFModel)
#include "G4GoudsmitSaundersonMscModel.hh"
DECLARE_GEANT4_PROCESS(G4GoudsmitSaundersonMscModel)
#include "G4hBremsstrahlungModel.hh"
DECLARE_GEANT4_PROCESS(G4hBremsstrahlungModel)
#include "G4hCoulombScatteringModel.hh"
DECLARE_GEANT4_PROCESS(G4hCoulombScatteringModel)
//#include "G4hNuclearStoppingModel.hh"
//DECLARE_GEANT4_PROCESS(G4hNuclearStoppingModel)
#include "G4hPairProductionModel.hh"
DECLARE_GEANT4_PROCESS(G4hPairProductionModel)
//#include "G4hParametrisedLossModel.hh"
//DECLARE_GEANT4_PROCESS(G4hParametrisedLossModel)
#include "G4ICRU49NuclearStoppingModel.hh"
DECLARE_GEANT4_PROCESS(G4ICRU49NuclearStoppingModel)
#include "G4ICRU73NoDeltaModel.hh"
DECLARE_GEANT4_PROCESS(G4ICRU73NoDeltaModel)
#include "G4ICRU73QOModel.hh"
DECLARE_GEANT4_PROCESS(G4ICRU73QOModel)
//#include "G4INCLIClusteringModel.hh"
//DECLARE_GEANT4_PROCESS(G4INCLIClusteringModel)
//#include "G4INCLIPropagationModel.hh"
//DECLARE_GEANT4_PROCESS(G4INCLIPropagationModel)
//#include "G4INCLStandardPropagationModel.hh"
//DECLARE_GEANT4_PROCESS(G4INCLStandardPropagationModel)
//#include "G4IonChuFluctuationModel.hh"
//DECLARE_GEANT4_PROCESS(G4IonChuFluctuationModel)

#endif


#if 0
#include ".hh"
DECLARE_GEANT4_PROCESS()
#endif

