
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "ILDExPhysicsList.h"

#include "G4ProcessManager.hh"
#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExPhysicsList::ILDExPhysicsList():  G4VUserPhysicsList()
{
  defaultCutValue = 1.0*mm;
  SetVerboseLevel(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ILDExPhysicsList::~ILDExPhysicsList()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExPhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  G4BosonConstructor  pBosonConstructor;
  pBosonConstructor.ConstructParticle();

  G4LeptonConstructor pLeptonConstructor;
  pLeptonConstructor.ConstructParticle();

//  G4MesonConstructor pMesonConstructor;
//  pMesonConstructor.ConstructParticle();
//
//  G4BaryonConstructor pBaryonConstructor;
//  pBaryonConstructor.ConstructParticle();
//
//  G4IonConstructor pIonConstructor;
//  pIonConstructor.ConstructParticle(); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExPhysicsList::ConstructProcess()
{
  AddTransportation();
  ConstructEM();
  ConstructDecay();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"

#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4hMultipleScattering.hh"
#include "G4hIonisation.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4ionIonisation.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExPhysicsList::ConstructEM()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "gamma") {
      // gamma         
//      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
//      pmanager->AddDiscreteProcess(new G4ComptonScattering);
//      pmanager->AddDiscreteProcess(new G4GammaConversion);
      
    } else if (particleName == "e-") {
      //electron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
//      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);      

    } else if (particleName == "e+") {
      //positron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4eIonisation,        -1, 2, 2);
//      pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3, 3);
//      pmanager->AddProcess(new G4eplusAnnihilation,   0,-1, 4);
    
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
      //muon  
      pmanager->AddProcess(new G4MuMultipleScattering,-1, 1, 1);
      pmanager->AddProcess(new G4MuIonisation,       -1, 2, 2);
      //      pmanager->AddProcess(new G4MuBremsstrahlung,   -1, 3, 3);
      //      pmanager->AddProcess(new G4MuPairProduction,   -1, 4, 4);
             
    } else if( particleName == "proton" ||
               particleName == "pi-" ||
               particleName == "pi+"    ) {
      //proton  
//      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
//      pmanager->AddProcess(new G4hIonisation,         -1, 2, 2);
//      pmanager->AddProcess(new G4hBremsstrahlung,     -1, 3, 3);
//      pmanager->AddProcess(new G4hPairProduction,     -1, 4, 4);       
     
    } else if( particleName == "alpha" || 
	       particleName == "He3" )     {
      //alpha 
//      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
//      pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);
     
    } else if( particleName == "GenericIon" ) { 
      //Ions 
//      pmanager->AddProcess(new G4hMultipleScattering, -1, 1, 1);
//      pmanager->AddProcess(new G4ionIonisation,       -1, 2, 2);     
      
      } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
//      pmanager->AddProcess(new G4hMultipleScattering,-1, 1, 1);
//      pmanager->AddProcess(new G4hIonisation,        -1, 2, 2);        
    }     
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Decay.hh"

void ILDExPhysicsList::ConstructDecay()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ILDExPhysicsList::SetCuts()
{
  if (verboseLevel >0){
    G4cout << "ILDExPhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  //
  SetCutValue(defaultCutValue, "gamma");
  SetCutValue(defaultCutValue, "e-");
  SetCutValue(defaultCutValue, "e+");
  //  SetCutValue(defaultCutValue, "proton");

  if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

