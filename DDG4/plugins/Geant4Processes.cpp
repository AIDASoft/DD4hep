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

//fg: uncomment to build for geant4 9.6
//    -> should be handled in cmake files...
//#define GEANT4_9_6



// ======================================================================
// Predefined physics constructors
//
//  Usage:
//
//  <physicslist name="Geant4PhysicsList/MyPhysics.0">
//    <physics>
//      <construct name="G4EmStandardPhysics"/>
//      <construct name="HadronPhysicsQGSP"/>
//    </physics>
//  </physicslist>
//
// ======================================================================

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
#include "G4EmLivermorePhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmLivermorePhysics)

// Ion and hadrons
#include "G4IonBinaryCascadePhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonBinaryCascadePhysics)

#ifdef GEANT4_9_6
 #include "G4IonINCLXXPhysics.hh"
 DECLARE_GEANT4_PHYSICS(G4IonINCLXXPhysics)
 #include "G4IonLHEPPhysics.hh"
 DECLARE_GEANT4_PHYSICS(G4IonLHEPPhysics)
#endif

#include "G4IonPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonPhysics)
#include "G4IonQMDPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonQMDPhysics)
#include "G4LHEPStoppingPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4LHEPStoppingPhysics)
#include "G4QStoppingPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QStoppingPhysics)
#include "G4HadronElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysics)
#include "G4HadronDElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronDElasticPhysics)
#include "G4HadronElasticPhysicsHP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsHP)
#include "G4HadronElasticPhysicsLEND.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsLEND)
#include "G4HadronElasticPhysicsLHEP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsLHEP)
#include "G4HadronElasticPhysicsXS.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsXS)
#include "G4HadronHElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronHElasticPhysics)
#include "G4HadronQElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronQElasticPhysics)
#include "G4NeutronTrackingCut.hh"
DECLARE_GEANT4_PHYSICS(G4NeutronTrackingCut)

// Optical physics
#include "G4OpticalPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4OpticalPhysics)

#include "G4QAtomicPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QAtomicPhysics)
#include "G4QCaptureAtRestPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QCaptureAtRestPhysics)
#include "G4QElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QElasticPhysics)
#include "G4QEmExtraPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QEmExtraPhysics)
#include "G4QNeutrinoPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QNeutrinoPhysics)
#include "G4QStoppingPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QStoppingPhysics)
#include "G4QIonPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4QIonPhysics)
#include "G4RadioactiveDecayPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4RadioactiveDecayPhysics)

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

// ======================================================================
// Predefined physics processes to be attached to particles
//
//  Usage:
//
//  <physicslist name="Geant4PhysicsList/MyPhysics.0">
//    <processes>
//      <particle name="e[+-]" cut="1*mm">
//	  <process name="G4eMultipleScattering" ordAtRestDoIt="-1" ordAlongSteptDoIt="1" ordPostStepDoIt="1"/>
//	  <process name="G4eIonisation"         ordAtRestDoIt="-1" ordAlongSteptDoIt="2" ordPostStepDoIt="2"/>
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


// ======================================================================
// Predefined physics lists
//
//  Usage:
//
//  <physicslist name="Geant4PhysicsList/MyPhysics.0">
//    <list name="TQGSP_FTFP_BERT_95"/>
//  </physicslist>
//
//
// ======================================================================
#include "DDG4/Geant4UserPhysicsList.h"
namespace {
  struct EmptyPhysics : public G4VModularPhysicsList {
    EmptyPhysics(int) {}
    virtual ~EmptyPhysics() {}
    virtual void ConstructProcess()      {}
    virtual void ConstructParticle()      {}
  };
}
DECLARE_GEANT4_PHYSICS_LIST(EmptyPhysics)

// Physics constructors from source/physics_lists
#include "CHIPS.hh"
DECLARE_GEANT4_PHYSICS_LIST(CHIPS)
#ifdef GEANT4_9_6

 #include "CHIPS_HP.hh"
 DECLARE_GEANT4_PHYSICS_LIST(CHIPS_HP)
 #include "FTFP_BERT.hh"
 DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT)
 #include "FTFP_BERT_HP.hh"
 DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_HP)
 #include "QGSP_BERT_95.hh"
 DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_95)
 #include "QGSP_BERT_95XS.hh"
 DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_95XS)
 #include "QGSP_INCLXX.hh"
 DECLARE_GEANT4_PHYSICS_LIST(QGSP_INCLXX)
 #include "HadronPhysicsQGSP_INCLXX.hh"
 DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_INCLXX)
 #include "HadronPhysicsFTFP_BERT.hh"
 DECLARE_GEANT4_PHYSICS(HadronPhysicsFTFP_BERT)
 #include "HadronPhysicsFTFP_BERT_HP.hh"
 DECLARE_GEANT4_PHYSICS(HadronPhysicsFTFP_BERT_HP)
#include "HadronPhysicsQGSP_BERT_95.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_95)
#include "HadronPhysicsQGSP_FTFP_BERT_95.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_FTFP_BERT_95)

#endif

#include "FTF_BIC.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTF_BIC)
#include "FTFP_BERT_EMV.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_EMV)
#include "FTFP_BERT_EMX.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_EMX)
#include "FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT)
#include "FTFP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS_LIST(FTFP_BERT_TRV)
#include "LBE.hh"
//DECLARE_GEANT4_PHYSICS_LIST(LBE) takes no verbosity arg!
#include "LHEP_EMV.hh"
DECLARE_GEANT4_PHYSICS_LIST(LHEP_EMV)
#include "LHEP.hh"
DECLARE_GEANT4_PHYSICS_LIST(LHEP)
#include "QBBC.hh"
DECLARE_GEANT4_PHYSICS_LIST(QBBC)
#include "QGS_BIC.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGS_BIC)
#include "QGSC_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSC_BERT)
#include "QGSC_CHIPS.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSC_CHIPS)
#include "QGSP_BERT_CHIPS.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_CHIPS)
#include "QGSP_BERT_EMV.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_EMV)
#include "QGSP_BERT_EMX.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_EMX)
#include "QGSP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT)
#include "QGSP_BERT_NOLEP.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_NOLEP)
#include "QGSP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BERT_TRV)
#include "QGSP_BIC_EMY.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BIC_EMY)
#include "QGSP_BIC_HP.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BIC_HP)
#include "QGSP_BIC.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_BIC)
#include "QGSP_FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_FTFP_BERT)
#include "QGSP.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP)
#include "QGSP_QEL.hh"
DECLARE_GEANT4_PHYSICS_LIST(QGSP_QEL)
#include "HadronPhysicsCHIPS.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsCHIPS)
#include "HadronPhysicsFTF_BIC.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsFTF_BIC)
#include "HadronPhysicsFTFP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsFTFP_BERT_TRV)
#include "HadronPhysicsLHEP_EMV.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsLHEP_EMV)
#include "HadronPhysicsLHEP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsLHEP)
#include "HadronPhysicsQGS_BIC.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGS_BIC)
#include "HadronPhysicsQGSC_BERT.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSC_BERT)
#include "HadronPhysicsQGSC_CHIPS.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSC_CHIPS)
#include "HadronPhysicsQGSP_BERT_CHIPS.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_CHIPS)
#include "HadronPhysicsQGSP_BERT.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT)
#include "HadronPhysicsQGSP_BERT_HP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_HP)
#include "HadronPhysicsQGSP_BERT_NOLEP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_NOLEP)
#include "HadronPhysicsQGSP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BERT_TRV)
#include "HadronPhysicsQGSP_BIC.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BIC)
#include "HadronPhysicsQGSP_BIC_HP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_BIC_HP)
#include "HadronPhysicsQGSP_FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP_FTFP_BERT)
#include "HadronPhysicsQGSP.hh"
DECLARE_GEANT4_PHYSICS(HadronPhysicsQGSP)

#if 0

#include ".hh"
DECLARE_GEANT4_PHYSICS_LIST()

#include ".hh"
DECLARE_GEANT4_PHYSICS()

#include ".hh"
DECLARE_GEANT4_PROCESS()
#endif
