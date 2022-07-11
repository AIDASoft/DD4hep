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
  auto* a = dynamic_cast<Geant4Action*>(this->m_parametrization);
  detail::releasePtr(a);
  this->m_parametrization = nullptr;
  detail::deletePtr(m_particleBounds);
  detail::deletePtr(m_showerModel);
  detail::deletePtr(m_hitMaker);
}

/// Adopt shower parametrization object
void Geant4GFlashShowerModel::adoptShowerParametrization(Geant4Action* action)   {
  if ( this->m_parametrization )    {
    auto* a = dynamic_cast<Geant4Action*>(this->m_parametrization);
    detail::releasePtr(a);
    this->m_parametrization = nullptr;
  }
  if ( action )   {
    this->m_parametrization = dynamic_cast<GVFlashShowerParameterisation*>(action);
    if ( !this->m_parametrization )   {
      except("The supplied parametrization %s was found as Geant4Action, but is no "
	     "GVFlashShowerParameterisation!", this->m_paramName.c_str());
    }
    action->addRef();
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
  G4Region* region = 0;
  Geant4Action* action = nullptr;
  if ( !this->m_parametrization && this->m_paramName.empty() )    {
    except("No proper parametrization name supplied in the properties: %s",this->m_paramName.c_str());
  }

  this->m_showerModel = new GFlashShowerModel(this->name(), region);
  if ( !this->m_parametrization )   {
    action = kernel.globalAction(this->m_paramName, false);
    this->adoptShowerParametrization(action);
  }
  this->m_hitMaker = new GFlashHitMaker();
  this->m_particleBounds = new GFlashParticleBounds();
  this->m_showerModel->SetParameterisation(*this->m_parametrization);
  this->m_showerModel->SetParticleBounds(*this->m_particleBounds);
  this->m_showerModel->SetHitMaker(*this->m_hitMaker);
}
