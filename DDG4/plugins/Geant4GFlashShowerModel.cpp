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
#include <G4FastSimulationPhysics.hh>
class GVFlashShowerParameterisation;
class G4ParticleDefinition;
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

    /// Geant4 wrapper for the GFlash shower model
    /**
     *  Geant4 wrapper for the GFlash shower model
     *  See e.g. the G4 example examples/extended/parameterisations/gflash/gflash3
     *  for details.
     *
     *  The parametrization is passed as a Geant4Action to be able to configure it,
     *  which is polymorph the GVFlashShowerParameterisation.
     *
     *  \author  M.Frank
     *
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4GFlashShowerModel : public Geant4DetectorConstruction    {
    protected:
      // typedef std::vector<std::pair<std::string, double> > ParticleConfig;
      typedef std::map<std::string, std::string> ParticleConfig;

      /// Property: Region name to which this parametrization should be applied
      std::string m_regionName    { "Region-name-not-specified"};
      /// Property: Name of the Geant4Action implementing the parametrization
      std::string m_paramName;
      /// Property: Material name for GFlashHomoShowerParameterisation
      std::string m_material;
      /// Property: Material 2 name for GFlashSamplingShowerParameterisation
      std::string m_material_2;
      /// Property: Parameter 1 name for GFlashSamplingShowerParameterisation
      double      m_parameter_1;
      /// Property: Parameter 2 name for GFlashSamplingShowerParameterisation
      double      m_parameter_2;

      /// Property: Parametrisation control: Enable GFlash (is disabled by default in Geant4)
      bool        m_enable                { false };
      /// Property: Defines if particle containment is checked
      int         m_particleCountainment  { 1 };
      /// Property: Defines step lenght
      double      m_stepX0                { 0.1 };
      /// Property: Set minimum kinetic energy to trigger parametrisation
      ParticleConfig m_eMin  { };
      /// Property: Set maximum kinetic energy to trigger parametrisation
      ParticleConfig m_eMax  { };
      /// Property: Set maximum kinetic energy for electrons to be killed
      ParticleConfig m_eKill { };

      /// Reference to the shower model
      GFlashShowerModel*             m_showerModel     { nullptr };
      /// Reference to the parametrization
      GVFlashShowerParameterisation* m_parametrization { nullptr };
      /// Reference to the particle bounds object
      GFlashParticleBounds*          m_particleBounds  { nullptr };
      /// Reference to the hit maker
      GFlashHitMaker*                m_hitMaker        { nullptr };

    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4GFlashShowerModel);

      /// Get parametrization material
      G4Material* getMaterial(const std::string& name)   const;

      /// Access particle definition from string
      G4ParticleDefinition* getParticleDefinition(const std::string& name)   const;

    public:
      /// Standard constructor
      Geant4GFlashShowerModel(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4GFlashShowerModel();

      /// Adopt shower parametrization object
      void adoptShowerParametrization(Geant4Action* param);

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
#include <DD4hep/Detector.h>
#include <DDG4/Geant4Action.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4Mapping.h>
#include <DD4hep/DD4hepUnits.h>

// Geant4 include files
#include <GVFlashShowerParameterisation.hh>
#include <GFlashHomoShowerParameterisation.hh>
#include <GFlashSamplingShowerParameterisation.hh>
#include <G4FastSimulationManager.hh>
#include <GFlashShowerModel.hh>
#include <GFlashHitMaker.hh>
#include <GFlashParticleBounds.hh>
#include <G4ParticleTable.hh>

#include <sstream>

using namespace dd4hep;
using namespace dd4hep::sim;

/// Standard constructor
Geant4GFlashShowerModel::Geant4GFlashShowerModel(Geant4Context* ctxt, const std::string& nam)
  : Geant4DetectorConstruction(ctxt, nam)
{
  declareProperty("RegionName",               m_regionName);
  declareProperty("Parametrization",          m_paramName);
  declareProperty("Material",                 m_material);
  declareProperty("Material_1",               m_material);
  declareProperty("Material_2",               m_material_2);
  declareProperty("Parameter_1",              m_parameter_1);
  declareProperty("Parameter_2",              m_parameter_2);
  declareProperty("Enable",                   m_enable);
  declareProperty("CheckParticleContainment", m_particleCountainment);
  declareProperty("StepLength",               m_stepX0);
  declareProperty("Emin",                     m_eMin);
  declareProperty("Emax",                     m_eMax);
  declareProperty("Ekill",                    m_eKill);
}

/// Default destructor
Geant4GFlashShowerModel::~Geant4GFlashShowerModel()    {
  if ( this->m_parametrization )   {
    auto* a = dynamic_cast<Geant4Action*>(this->m_parametrization);
    if ( a ) detail::releasePtr(a);
    else detail::deletePtr(this->m_parametrization);
  }
  this->m_parametrization = nullptr;
  detail::deletePtr(m_particleBounds);
  detail::deletePtr(m_showerModel);
  //detail::deletePtr(m_hitMaker);
  m_hitMaker = nullptr;
}

/// Access particle definition from string
G4ParticleDefinition* Geant4GFlashShowerModel::getParticleDefinition(const std::string& particle)  const  {
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* def = pt->FindParticle(particle);
  if ( def ) return def;
  except("Failed to access Geant4 particle definition: %s", particle.c_str());
  return nullptr;
}

G4Material* Geant4GFlashShowerModel::getMaterial(const std::string& mat_name)   const   {
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

/// Adopt shower parametrization object
void Geant4GFlashShowerModel::adoptShowerParametrization(Geant4Action* action)   {
  if ( this->m_parametrization )    {
    auto* a = dynamic_cast<Geant4Action*>(this->m_parametrization);
    this->m_parametrization = nullptr;
    detail::releasePtr(a);
  }
  if ( action )   {
    this->m_parametrization = dynamic_cast<GVFlashShowerParameterisation*>(action);
    if ( this->m_parametrization )   {
      action->addRef();
      return;
    }
    except("The supplied parametrization %s was found as Geant4Action, but is no "
	   "GVFlashShowerParameterisation!", this->m_paramName.c_str());
  }
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
  Geant4GeometryInfo& mapping = Geant4Mapping::instance().data();
  Region rg = kernel.detectorDescription().region(this->m_regionName);
  if ( !rg.isValid() )   {
    except("Failed to access the region %s from the detector description.", this->m_regionName.c_str());
  }
  auto region_iter = mapping.g4Regions.find(rg);
  if ( region_iter == mapping.g4Regions.end() )    {
    except("Failed to locate G4Region: %s from the Geant4 mapping.");
  }
  G4Region* region = (*region_iter).second;
  this->m_showerModel = new GFlashShowerModel(this->name(), region);
  std::stringstream logger;
  logger << "Geant4 shower model initialized with parametrization: ";
  if ( !this->m_parametrization )   {
    if ( this->m_paramName.empty() )    {
      except("No proper parametrization name supplied in the properties: %s",this->m_paramName.c_str());
    }
    if ( this->m_paramName == "GFlashHomoShowerParameterisation" )    {
      G4Material* mat = this->getMaterial(m_material);
      this->m_parametrization = new GFlashHomoShowerParameterisation(mat, nullptr);
      logger << "GFlashHomoShowerParameterisation Material: " << mat->GetName();
    }
    else if ( this->m_paramName == "GFlashSamplingShowerParameterisation" )    {
      G4Material* mat1 = this->getMaterial(m_material);
      G4Material* mat2 = this->getMaterial(m_material_2);
      this->m_parametrization = 
	new GFlashSamplingShowerParameterisation(mat1, mat2, m_parameter_1, m_parameter_2, nullptr);
      logger << "GFlashSamplingShowerParameterisation Material: " << mat1->GetName()
	     << "  " << mat2->GetName() << " Params: " << m_parameter_1 << " " << m_parameter_2;
    }
    else   {
      auto* action = kernel.globalAction(this->m_paramName, false);
      this->adoptShowerParametrization(action);
      if ( action ) logger << typeName(typeid(*action));
    }
  }
  else   {
    logger << typeName(typeid(*this->m_parametrization));
  }
  if ( !this->m_parametrization )    {
    except("No proper parametrization supplied. Failed to construct shower model.");
  }
  this->m_hitMaker = new GFlashHitMaker();
  this->m_particleBounds = new GFlashParticleBounds();
  this->m_showerModel->SetParameterisation(*this->m_parametrization);
  this->m_showerModel->SetParticleBounds(*this->m_particleBounds);
  this->m_showerModel->SetHitMaker(*this->m_hitMaker);

  /// Now configure the shower model:
  this->m_showerModel->SetFlagParamType(this->m_enable ? 1 : 0);
  this->m_showerModel->SetFlagParticleContainment(this->m_particleCountainment);
  this->m_showerModel->SetStepInX0(this->m_stepX0);

  for(const auto& prop : this->m_eMin)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) * dd4hep::GeV/CLHEP::GeV;
    this->m_particleBounds->SetMinEneToParametrise(*def, val);
    this->info("SetMinEneToParametrise [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }
  for(const auto& prop : this->m_eMax)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) * dd4hep::GeV/CLHEP::GeV;
    this->m_particleBounds->SetMaxEneToParametrise(*def, val);
    this->info("SetMaxEneToParametrise [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }
  for(const auto& prop : this->m_eKill)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) * dd4hep::GeV/CLHEP::GeV;
    this->m_particleBounds->SetEneToKill(*def, val);
    this->info("SetEneToKill           [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }
  this->info(logger.str().c_str());
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(Geant4GFlashShowerModel)
