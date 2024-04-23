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
#ifndef DDG4_GEANT4GFLASHSHOWERMODEL_H
#define DDG4_GEANT4GFLASHSHOWERMODEL_H

// Framework include files
#include <DDG4/Geant4FastSimShowerModel.h>

// Geant4 include files
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
    class Geant4GFlashShowerModel : public Geant4FastSimShowerModel   {
    protected:
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

      /// Property: Defines if particle containment is checked
      int         m_particleContainment { 1 };
      /// Property: Defines step lenght
      double      m_stepX0              { 0.1 };

      /// Reference to the parametrization
      GVFlashShowerParameterisation* m_parametrization { nullptr };
      /// Reference to the particle bounds object
      GFlashParticleBounds*          m_particleBounds  { nullptr };
      /// Reference to the hit maker
      GFlashHitMaker*                m_hitMaker        { nullptr };

    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4GFlashShowerModel);

    public:
      /// Standard constructor
      Geant4GFlashShowerModel(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4GFlashShowerModel();

      /// Adopt shower parametrization object
      void adoptShowerParametrization(Geant4Action* param);

      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt);
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif // DDG4_GEANT4GFLASHSHOWERMODEL_H

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
// #include <DDG4/Geant4GFlashShowerModel.h>
#include <DD4hep/Detector.h>
#include <DDG4/Geant4Action.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4Mapping.h>

// Geant4 include files
#include <GVFlashShowerParameterisation.hh>
#include <GFlashHomoShowerParameterisation.hh>
#include <GFlashSamplingShowerParameterisation.hh>
#include <GFlashParticleBounds.hh>
#include <GFlashShowerModel.hh>
#include <GFlashHitMaker.hh>

// C/C++ include files
#include <sstream>

using namespace dd4hep::sim;

/// Standard constructor
Geant4GFlashShowerModel::Geant4GFlashShowerModel(Geant4Context* ctxt, const std::string& nam)
  : Geant4FastSimShowerModel(ctxt, nam)
{
  declareProperty("Parametrization",          m_paramName);
  declareProperty("Material",                 m_material);
  declareProperty("Material_1",               m_material);
  declareProperty("Material_2",               m_material_2);
  declareProperty("Parameter_1",              m_parameter_1);
  declareProperty("Parameter_2",              m_parameter_2);
  declareProperty("CheckParticleContainment", m_particleContainment);
  this->m_applicablePartNames.emplace_back("e+");
  this->m_applicablePartNames.emplace_back("e-");
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
  //detail::deletePtr(m_hitMaker);
  m_hitMaker = nullptr;
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
    except("Failed to locate G4Region: %s from the Geant4 mapping.", this->m_regionName.c_str());
  }
  G4Region* region = (*region_iter).second;
  
  std::stringstream logger;
  auto* shower_model = new GFlashShowerModel(this->name(), region);
  this->m_model = shower_model;
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
      logger << "GFlashSamplingShowerParameterisation Materials: " << mat1->GetName()
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
  shower_model->SetHitMaker(*this->m_hitMaker);
  this->m_particleBounds = new GFlashParticleBounds();
  shower_model->SetParticleBounds(*this->m_particleBounds);
  shower_model->SetParameterisation(*this->m_parametrization);

  /// Now configure the shower model:
  shower_model->SetStepInX0(this->m_stepX0);
  shower_model->SetFlagParamType(this->m_enable ? 1 : 0);
  shower_model->SetFlagParticleContainment(this->m_particleContainment);

  for(const auto& prop : this->m_eMin)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) / CLHEP::GeV;
    this->m_particleBounds->SetMinEneToParametrise(*def, val);
    this->info("SetMinEneToParametrise [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }
  for(const auto& prop : this->m_eMax)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) / CLHEP::GeV;
    this->m_particleBounds->SetMaxEneToParametrise(*def, val);
    this->info("SetMaxEneToParametrise [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }
  for(const auto& prop : this->m_eKill)    {
    G4ParticleDefinition* def = this->getParticleDefinition(prop.first);
    double val = dd4hep::_toDouble(prop.second) / CLHEP::GeV;
    this->m_particleBounds->SetEneToKill(*def, val);
    this->info("SetEneToKill           [%-16s] = %8.4f GeV", prop.first.c_str(), val);
  }

  this->addShowerModel(region);
  this->info(logger.str().c_str());
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(Geant4GFlashShowerModel)
