// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// EM physics
#include "G4EmStandardPhysics_option1.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics_option1)
#include "G4EmStandardPhysics_option2.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics_option2)
#include "G4EmStandardPhysics_option3.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics_option3)
#include "G4EmStandardPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysics)
#include "G4EmStandardPhysicsSS.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysicsSS)
#include "G4EmStandardPhysicsWVI.hh"
DECLARE_GEANT4_PHYSICS(G4EmStandardPhysicsWVI)
#include "G4EmExtraPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmExtraPhysics)
#include "G4EmDNAPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmDNAPhysics)
#include "G4EmLowEPPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmLowEPPhysics)
#include "G4EmLivermorePhysics.hh"
DECLARE_GEANT4_PHYSICS(G4EmLivermorePhysics)
#include "G4ChargeExchangePhysics.hh"
DECLARE_GEANT4_PHYSICS(G4ChargeExchangePhysics)
#include "G4GenericBiasingPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4GenericBiasingPhysics)

#include "G4HadronDElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronDElasticPhysics)
#include "G4HadronElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysics)
#include "G4HadronElasticPhysicsHP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsHP)
#include "G4HadronElasticPhysicsLEND.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsLEND)
#include "G4HadronElasticPhysicsPHP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsPHP)
#include "G4HadronElasticPhysicsXS.hh"
DECLARE_GEANT4_PHYSICS(G4HadronElasticPhysicsXS)
#include "G4HadronHElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4HadronHElasticPhysics)
#include "G4HadronPhysicsFTF_BIC.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsFTF_BIC)
#include "G4HadronPhysicsFTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsFTFP_BERT)
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsFTFP_BERT_HP)
#include "G4HadronPhysicsFTFP_BERT_TRV.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsFTFP_BERT_TRV)
#include "G4HadronPhysicsINCLXX.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsINCLXX)
#include "G4HadronPhysicsNuBeam.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsNuBeam)
#include "G4HadronPhysicsQGS_BIC.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsQGS_BIC)
#include "G4HadronPhysicsQGSP_BERT.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsQGSP_BERT)
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsQGSP_BERT_HP)
#include "G4HadronPhysicsQGSP_BIC_AllHP.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsQGSP_BIC_AllHP)
#include "G4HadronPhysicsQGSP_FTFP_BERT.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsQGSP_FTFP_BERT)
#include "G4HadronPhysicsShielding.hh"
DECLARE_GEANT4_PHYSICS(G4HadronPhysicsShielding)


// Ion and hadrons
#include "G4IonBinaryCascadePhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonBinaryCascadePhysics)
#include "G4IonElasticPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonElasticPhysics)

#include "G4IonINCLXXPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonINCLXXPhysics)
#include "G4IonPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonPhysics)
#include "G4IonQMDPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4IonQMDPhysics)

#include "G4RadioactiveDecayPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4RadioactiveDecayPhysics)
#include "G4StepLimiterPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4StepLimiterPhysics)
#include "G4StoppingPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4StoppingPhysics)

#include "G4NeutronTrackingCut.hh"
DECLARE_GEANT4_PHYSICS(G4NeutronTrackingCut)

// Optical physics
#include "G4OpticalPhysics.hh"
DECLARE_GEANT4_PHYSICS(G4OpticalPhysics)
