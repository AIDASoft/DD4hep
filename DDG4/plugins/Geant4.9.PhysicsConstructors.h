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

#if G4VERSION_NUMBER<=960
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

#if G4VERSION_NUMBER<=960
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
