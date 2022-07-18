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

// Framework include files
#include <DDG4/Geant4FastSimShowerModel.h>
#include <DDG4/Geant4Mapping.h>
#include <DDG4/Geant4Kernel.h>

// Geant4 include files
#include <G4FastSimulationManager.hh>
#include <G4VFastSimulationModel.hh>
#include <G4TouchableHandle.hh>
#include <G4ParticleTable.hh>
#include <G4FastStep.hh>

// C/C++ include files
#include <sstream>

using namespace dd4hep;
using namespace dd4hep::sim;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Forward declaratons
    class Geant4FastSimShowerModel;
    
    /// Geant4 wrapper for the Geant4 fast simulation shower model
    /**
     *  Geant4 wrapper for the Geant4 fast simulation shower model
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ShowerModelWrapper : public G4VFastSimulationModel  {
      /// Reference to the model wrapper
      Geant4FastSimShowerModel* m_model { nullptr };

    public:
      /// Initializing constructor
      Geant4ShowerModelWrapper(Geant4FastSimShowerModel* model);
      /// Default destructor
      virtual ~Geant4ShowerModelWrapper() = default;
      /// User callback to determine if the model is applicable for the particle type
      virtual G4bool IsApplicable(const G4ParticleDefinition& particle)  override;
      /// User callback to determine if the shower creation should be triggered
      virtual G4bool ModelTrigger(const G4FastTrack& track)  override;
      /// User callback to model the particle/energy shower
      virtual void DoIt(const G4FastTrack& track, G4FastStep& step)  override;
    };
    
  }
}

/// Initializing constructor
Geant4ShowerModelWrapper::Geant4ShowerModelWrapper(Geant4FastSimShowerModel* model)
  : G4VFastSimulationModel(model->name()), m_model(model)
{
}

/// User callback to determine if the model is applicable for the particle type
G4bool Geant4ShowerModelWrapper::IsApplicable(const G4ParticleDefinition& particle)    {
  return this->m_model->check_applicability(particle);
}

/// User callback to determine if the shower creation should be triggered
G4bool Geant4ShowerModelWrapper::ModelTrigger(const G4FastTrack& track)   {
  return this->m_model->check_trigger(track);
}

/// User callback to model the particle/energy shower
void Geant4ShowerModelWrapper::DoIt(const G4FastTrack& track, G4FastStep& step)   {
  this->m_model->modelShower(track, step);
}

/// Standard constructor
Geant4FastSimShowerModel::Geant4FastSimShowerModel(Geant4Context* ctxt, const std::string& nam)
  : Geant4DetectorConstruction(ctxt, nam)
{
  this->declareProperty("RegionName",          this->m_regionName);
  this->declareProperty("ApplicableParticles", this->m_applicablePartNames);
  this->declareProperty("Enable",              this->m_enable);
  this->declareProperty("StepLength",          this->m_stepX0);
  this->declareProperty("Emin",                this->m_eMin);
  this->declareProperty("Emax",                this->m_eMax);
  this->declareProperty("Ekill",               this->m_eKill);
  this->declareProperty("Etrigger",            this->m_eTriggerNames);
  this->m_wrapper.reset(new Geant4ShowerModelWrapper(this));
}

/// Default destructor
Geant4FastSimShowerModel::~Geant4FastSimShowerModel()    {
  detail::deletePtr(m_model);
  m_wrapper.reset();
}

/// Access particle definition from string
G4ParticleDefinition* Geant4FastSimShowerModel::getParticleDefinition(const std::string& particle)  const  {
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* def = pt->FindParticle(particle);
  if ( def ) return def;
  except("Failed to access Geant4 particle definition: %s", particle.c_str());
  return nullptr;
}

G4Material* Geant4FastSimShowerModel::getMaterial(const std::string& mat_name)   const   {
  auto& kernel = this->context()->kernel();
  Geant4GeometryInfo& mapping = Geant4Mapping::instance().data();
  Material material = kernel.detectorDescription().material(mat_name);
  if ( material.isValid() )   {
    auto mat_iter = mapping.g4Materials.find(material);
    if ( mat_iter != mapping.g4Materials.end() )   {
      return (*mat_iter).second;
    }
  }
  except("Failed to access shower parametrization material: %s", mat_name.c_str());
  return nullptr;
}

/// Access the region from the detector description by name
G4Region* Geant4FastSimShowerModel::getRegion(const std::string& nam)   const   {
  auto& kernel = this->context()->kernel();
  Geant4GeometryInfo& mapping = Geant4Mapping::instance().data();
  Region rg = kernel.detectorDescription().region(nam);
  if ( !rg.isValid() )   {
    except("Failed to access the region %s from the detector description.", nam.c_str());
  }
  auto region_iter = mapping.g4Regions.find(rg);
  if ( region_iter == mapping.g4Regions.end() )    {
    except("Failed to locate G4Region: %s from the Geant4 mapping.", nam.c_str());
  }
  G4Region* region = (*region_iter).second;
  if ( region )   {
  }
  return region;
}

/// Add shower model to region's fast simulation manager
void Geant4FastSimShowerModel::addShowerModel(G4Region* region)     {
  if ( !region )     {
    except("Geant4FastSimShowerModel::addShowerModel: Invalid G4Region reference!");
  }
  // Retrieves the Fast Simulation Manage and add the model
  G4FastSimulationManager* fastsimManager = region->GetFastSimulationManager();
  if ( !fastsimManager )   {
    fastsimManager = new G4FastSimulationManager(region, true);
  }
  // add this model to the Fast Simulation Manager.
  if ( m_model )
    fastsimManager->AddFastSimulationModel(m_model);
  else if ( m_wrapper )
    fastsimManager->AddFastSimulationModel(m_wrapper.get());
  else
    except("Geant4FastSimShowerModel::addShowerModel: Invalid shower model reference!");
}

/// Geometry construction callback. Called at "Construct()"
void Geant4FastSimShowerModel::constructGeo(Geant4DetectorConstructionContext* /* ctxt */)    {
  this->m_applicableParticles.clear();
  for(const std::string& p : m_applicablePartNames)   {
    G4ParticleDefinition* def = (p=="*") ? nullptr : this->getParticleDefinition(p);
    this->m_applicableParticles.emplace(def);
  }
}

/// Electromagnetic field construction callback. Called at "ConstructSDandField()"
void Geant4FastSimShowerModel::constructField(Geant4DetectorConstructionContext* /* ctxt */)   {
}

/// Sensitive detector construction callback. Called at "ConstructSDandField()"
void Geant4FastSimShowerModel::constructSensitives(Geant4DetectorConstructionContext* /* ctxt */)    {
  G4Region* region = this->getRegion(this->m_regionName);
  for(const auto& prop : this->m_eTriggerNames)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) * dd4hep::GeV/CLHEP::GeV;
    this->m_eTriggerCut.emplace(def, val);
    this->info("Set Energy(ModelTrigger) [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }
  this->m_model = nullptr;
  this->addShowerModel(region);
}

/// Kill primary particle when creating the shower
void Geant4FastSimShowerModel::killParticle(G4FastStep& step, double deposit, double step_length)   {
  step.KillPrimaryTrack();
  step.ProposePrimaryTrackPathLength(step_length);
  step.ProposeTotalEnergyDeposited(deposit);
}

/// User callback to determine if the model is applicable for the particle type
bool Geant4FastSimShowerModel::check_applicability(const G4ParticleDefinition& particle)   {
  return
    this->m_enable && 
    this->m_applicableParticles.find(&particle) != this->m_applicableParticles.end();
}

/// User callback to determine if the shower creation should be triggered
bool Geant4FastSimShowerModel::check_trigger(const G4FastTrack& track)    {
  auto* prim = track.GetPrimaryTrack();
  auto* def  = prim->GetParticleDefinition();
  auto  iter = this->m_eTriggerCut.find(def);
  if ( iter != this->m_eTriggerCut.end() )  {
    return (*iter).second < prim->GetKineticEnergy();
  }
  iter = this->m_eTriggerCut.find(nullptr);
  if ( iter != this->m_eTriggerCut.end() )  {
    return (*iter).second < prim->GetKineticEnergy();
  }
  return false;
}

/// User callback to model the particle/energy shower
void Geant4FastSimShowerModel::modelShower(const G4FastTrack& /* track */, G4FastStep& /* step */)    {
  except("Method %s::modelShower(const G4FastTrack& track, G4FastStep& step) "
	 "is not implemented. User implementation mandatory.", this->name().c_str());
}
