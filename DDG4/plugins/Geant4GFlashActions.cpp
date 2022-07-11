//==========================================================================
//  AIDA Detector description implementation
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

#ifndef DDG4_GEANT4GFLASHACTION_H
#define DDG4_GEANT4GFLASHACTION_H

// Framework include files
#include <DDG4/Geant4DetectorConstruction.h>

// Geant4 include files
#include "G4FastSimulationPhysics.hh"
class GVFlashShowerParameterisation;
class GFlashParticleBounds;
class GFlashShowerModel;
class GFlashHitMaker;

// C/C++ include files
#include <vector>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {
    
    class Geant4InputAction;

    /// Steering class for Geant4 GFlash parametrization
    /**
     *  Steering class for Geant4 GFlash parametrization
     *
     *  \author  M.Frank
     *
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GFlashShowerModel : public Geant4DetectorConstruction    {
    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4GFlashShowerModel);

      /// Region name to which this parametrization should be applied
      std::string m_regionName;
      /// Name of the shower model constructor
      std::string m_modelName;
      std::string m_paramName;

      GFlashShowerModel*             m_showerModel     { nullptr };
      GVFlashShowerParameterisation* m_parametrization { nullptr };
      GFlashParticleBounds*          m_particleBounds  { nullptr };
      GFlashHitMaker*                m_hitMaker        { nullptr };

    public:
      /// Standard constructor
      Geant4GFlashShowerModel(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4GFlashShowerModel();

      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt);
      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt);
      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt);
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_GEANT4GFLASHACTION_H

//==========================================================================
//  AIDA Detector description implementation
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

// #include <DDG4/Geant4GFlashShowerModel.h>
// Framework include files
#include <DDG4/Geant4Action.h>
#include <DDG4/Geant4Kernel.h>

// Geant4 include files
#include "GVFlashShowerParameterisation.hh"
#include "G4FastSimulationManager.hh"
#include "GFlashShowerModel.hh"
#include "GFlashHitMaker.hh"
#include "GFlashParticleBounds.hh"

using namespace dd4hep;
using namespace dd4hep::sim;

/// Standard constructor
Geant4GFlashShowerModel::Geant4GFlashShowerModel(Geant4Context* ctxt, const std::string& nam)
  : Geant4DetectorConstruction(ctxt, nam)
{
  declareProperty("Region",          m_regionName);
  declareProperty("Parametrization", m_paramName);
  declareProperty("Model",           m_modelName);
}

/// Default destructor
Geant4GFlashShowerModel::~Geant4GFlashShowerModel()    {
  detail::deletePtr(m_showerModel);
  detail::deletePtr(m_particleBounds);
  detail::deletePtr(m_hitMaker);
}

/// Geometry construction callback. Called at "Construct()"
void Geant4GFlashShowerModel::constructGeo(Geant4DetectorConstructionContext* /* ctxt */)    {
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4GFlashShowerModel::constructField(Geant4DetectorConstructionContext* /* ctxt */)   {
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4GFlashShowerModel::constructSensitives(Geant4DetectorConstructionContext* /* ctxt */)    {
  auto& kernel = this->context()->kernel();
  G4Region* region = 0;
  Geant4Action* action = nullptr;
  if ( !this->m_parametrization && this->m_paramName.empty() )    {
    except("No proper parametrization name supplied in the properties: %s",this->m_paramName.c_str());
  }

  this->m_showerModel = new GFlashShowerModel(this->name(), region);
  if ( !this->m_parametrization )   {
    action = kernel.globalAction(this->m_paramName, false);
    if ( action )   {
      this->m_parametrization = dynamic_cast<GVFlashShowerParameterisation*>(action);
      if ( !this->m_parametrization )   {
	except("The supplied parametrization %s was found as Geant4Action, but is no "
	       "GVFlashShowerParameterisation!", this->m_paramName.c_str());
      }
    }
  }
  this->m_hitMaker = new GFlashHitMaker();
  this->m_particleBounds = new GFlashParticleBounds();
  this->m_showerModel->SetParameterisation(*this->m_parametrization);
  this->m_showerModel->SetParticleBounds(*this->m_particleBounds);
  this->m_showerModel->SetHitMaker(*this->m_hitMaker);
}

#if 0
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material*          pbWO4 = nistManager->FindOrBuildMaterial("G4_PbWO4");
  // -- fast simulation models:
  // **********************************************
  // * Initializing shower modell
  // ***********************************************
  G4cout << "Creating shower parameterization models" << G4endl;
  fFastShowerModel = new GFlashShowerModel("fFastShowerModel", fRegion);
  fParameterisation = new GFlashHomoShowerParameterisation(pbWO4);
  fFastShowerModel->SetParameterisation(*fParameterisation);
  // Energy Cuts to kill particles:
  fParticleBounds = new GFlashParticleBounds();
  fFastShowerModel->SetParticleBounds(*fParticleBounds);
  // Makes the EnergieSpots
  fHitMaker = new GFlashHitMaker();
  fFastShowerModel->SetHitMaker(*fHitMaker);
#endif


#if 0
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
main()   {
  // -- Create a fast simulation physics constructor, used to augment
  // -- the above physics list to allow for fast simulation:
  G4FastSimulationPhysics* fastSimulationPhysics = new G4FastSimulationPhysics();
  // -- We now configure the fastSimulationPhysics object.
  // -- The gflash model (GFlashShowerModel, see ExGflashDetectorConstruction.cc)
  // -- is applicable to e+ and e- : we augment the physics list for these
  // -- particles (by adding a G4FastSimulationManagerProcess with below's
  // -- calls), this will make the fast simulation to be activated:
  fastSimulationPhysics->ActivateFastSimulation("e-");
  fastSimulationPhysics->ActivateFastSimulation("e+");
  // -- Register this fastSimulationPhysics to the physicsList,
  // -- when the physics list will be called by the run manager
  // -- (will happen at initialization of the run manager)
  // -- for physics process construction, the fast simulation
  // -- configuration will be applied as well.
  physicsList->RegisterPhysics( fastSimulationPhysics );
}

//------------------------------------------------------------------------------------------------
void ExGflash1DetectorConstruction::ConstructSDandField()
{
  // -- sensitive detectors:
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  ExGflash1SensitiveDetector* CaloSD
    = new ExGflash1SensitiveDetector("Calorimeter",this);
  SDman->AddNewDetector(CaloSD);
  fCrystalLog->SetSensitiveDetector(CaloSD);
  
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material*          pbWO4 = nistManager->FindOrBuildMaterial("G4_PbWO4");
  // -- fast simulation models:
  // **********************************************
  // * Initializing shower modell
  // ***********************************************
  G4cout << "Creating shower parameterization models" << G4endl;
  fFastShowerModel = new GFlashShowerModel("fFastShowerModel", fRegion);
  fParameterisation = new GFlashHomoShowerParameterisation(pbWO4);
  fFastShowerModel->SetParameterisation(*fParameterisation);
  // Energy Cuts to kill particles:
  fParticleBounds = new GFlashParticleBounds();
  fFastShowerModel->SetParticleBounds(*fParticleBounds);
  // Makes the EnergieSpots
  fHitMaker = new GFlashHitMaker();
  fFastShowerModel->SetHitMaker(*fHitMaker);
  G4cout<<"end shower parameterization."<<G4endl;
  // **********************************************
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
main()   {
  auto detector = new ExGflash2DetectorConstruction();
  detector->RegisterParallelWorld(new ExGflash2ParallelWorld("parallelWorld"));
  runManager->SetUserInitialization(detector);

  // G4cout<<"# GFlash Example: Physics "<<G4endl;
  // -- Select a physics list:
  G4VModularPhysicsList* physicsList = new FTFP_BERT();
  // -- Create a fast simulation physics constructor, used to augment
  // -- the above physics list to allow for fast simulation:
  G4FastSimulationPhysics* fastSimulationPhysics = new G4FastSimulationPhysics();
  // -- We now configure the fastSimulationPhysics object.
  // -- The gflash model (GFlashShowerModel, see ExGflashDetectorConstruction.cc)
  // -- is applicable to e+ and e- : we augment the physics list for these
  // -- particles (by adding a G4FastSimulationManagerProcess with below's
  // -- calls), this will make the fast simulation to be activated:
  fastSimulationPhysics->ActivateFastSimulation("e-", "parallelWorld");
  fastSimulationPhysics->ActivateFastSimulation("e+", "parallelWorld");
  // -- Register this fastSimulationPhysics to the physicsList,
  // -- when the physics list will be called by the run manager
  // -- (will happen at initialization of the run manager)
  // -- for physics process construction, the fast simulation
  // -- configuration will be applied as well.
  physicsList->RegisterPhysics( fastSimulationPhysics );
  runManager->SetUserInitialization(physicsList);

  // Action initialization:
  runManager->SetUserInitialization(new ExGflashActionInitialization);
}

//------------------------------------------------------------------------------------------------
void ExGflash2ParallelWorld::ConstructSD()
{
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material*          pbWO4 = nistManager->FindOrBuildMaterial("G4_PbWO4");
  // -- fast simulation models:
  // **********************************************
  // * Initializing shower modell
  // ***********************************************
  fFastShowerModel = new GFlashShowerModel("fFastShowerModel", fRegion);
  fParameterisation = new GFlashHomoShowerParameterisation(pbWO4);
  fFastShowerModel->SetParameterisation(*fParameterisation);
  // Energy Cuts to kill particles:
  fParticleBounds = new GFlashParticleBounds();
  fFastShowerModel->SetParticleBounds(*fParticleBounds);
  // Makes the EnergieSpots
  fHitMaker = new GFlashHitMaker();
  fFastShowerModel->SetHitMaker(*fHitMaker);
}

//------------------------------------------------------------------------------------------------
void ExGflash2DetectorConstruction::ConstructSDandField()
{
  // -- sensitive detectors:
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  ExGflash2SensitiveDetector* CaloSD
    = new ExGflash2SensitiveDetector("Calorimeter",this);
  SDman->AddNewDetector(CaloSD);
  fCrystalLog->SetSensitiveDetector(CaloSD);
  
  // Fast simulation implemented in ExGflash2ParallelWorld
}


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
main() {
  auto detector = new ExGflash3DetectorConstruction();
  detector->RegisterParallelWorld(new ExGflash3ParallelWorld("parallelWorld"));
  runManager->SetUserInitialization(detector);

  // G4cout<<"# GFlash Example: Physics "<<G4endl;
  // -- Select a physics list:
  G4VModularPhysicsList* physicsList = new FTFP_BERT();
  // -- Create a fast simulation physics constructor, used to augment
  // -- the above physics list to allow for fast simulation:
  G4FastSimulationPhysics* fastSimulationPhysics = new G4FastSimulationPhysics();
  // -- We now configure the fastSimulationPhysics object.
  // -- The gflash model (GFlashShowerModel, see ExGflashDetectorConstruction.cc)
  // -- is applicable to e+ and e- : we augment the physics list for these
  // -- particles (by adding a G4FastSimulationManagerProcess with below's
  // -- calls), this will make the fast simulation to be activated:
  fastSimulationPhysics->ActivateFastSimulation("e-");
  fastSimulationPhysics->ActivateFastSimulation("e+");
  // -- Register this fastSimulationPhysics to the physicsList,
  // -- when the physics list will be called by the run manager
  // -- (will happen at initialization of the run manager)
  // -- for physics process construction, the fast simulation
  // -- configuration will be applied as well.
  physicsList->RegisterPhysics( new G4ParallelWorldPhysics("parallelWorld") );
  physicsList->RegisterPhysics( fastSimulationPhysics );
  runManager->SetUserInitialization(physicsList);
}

//------------------------------------------------------------------------------------------------
void ExGflash3ParallelWorld::ConstructSD()
{
  // -- sensitive detectors:
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  ExGflash3SensitiveDetector* CaloSD
    = new ExGflash3SensitiveDetector("Calorimeter",this);
  SDman->AddNewDetector(CaloSD);
  fCrystalLog->SetSensitiveDetector(CaloSD);
}
//------------------------------------------------------------------------------------------------
void ExGflash3DetectorConstruction::ConstructSDandField()
{
  // SD moved to parallel world
  // -- fast simulation models:
  // **********************************************
  // * Initializing shower modell
  // ***********************************************
  fFastShowerModel = new GFlashShowerModel("fFastShowerModel", fRegion);
  fParameterisation = new GFlashHomoShowerParameterisation(fRegion->GetMaterialIterator()[0]);
  fFastShowerModel->SetParameterisation(*fParameterisation);
  // Energy Cuts to kill particles:
  fParticleBounds = new GFlashParticleBounds();
  fFastShowerModel->SetParticleBounds(*fParticleBounds);
  // Makes the EnergieSpots
  fHitMaker = new GFlashHitMaker();
  // Important: use SD defined in different geometry
  fHitMaker->SetNameOfWorldWithSD("parallelWorld");
  fFastShowerModel->SetHitMaker(*fHitMaker);
}
//------------------------------------------------------------------------------------------------
#endif

