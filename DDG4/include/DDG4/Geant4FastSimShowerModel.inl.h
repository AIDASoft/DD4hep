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
#ifndef DDG4_GEANT4FASTSIMSHOWERMODEL_INL_H
#define DDG4_GEANT4FASTSIMSHOWERMODEL_INL_H

// Framework include files
#include <DDG4/Geant4FastSimShowerModel.h>

// Geant4 include files

// C/C++ include files
#include <vector>
#include <memory>

/// Forward declarations

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Standard constructor
    template <typename CONCRETE_MODEL> 
    Geant4FSShowerModel<CONCRETE_MODEL>::Geant4FSShowerModel(Geant4Context* context, const std::string& nam)
      : Geant4FastSimShowerModel(context, nam)
    {
      this->initialize();
    }
    
    /// Default destructor
    template <typename CONCRETE_MODEL> 
    Geant4FSShowerModel<CONCRETE_MODEL>::~Geant4FSShowerModel()   {
    }

    /// Declare optional properties from embedded structure
    template <typename CONCRETE_MODEL> 
    void Geant4FSShowerModel<CONCRETE_MODEL>::initialize()     {
    }

    /// Geometry construction callback. Called at "Construct()"
    template <typename CONCRETE_MODEL> 
    void Geant4FSShowerModel<CONCRETE_MODEL>::constructGeo(Geant4DetectorConstructionContext* ctxt)   {
      this->Geant4FastSimShowerModel::constructGeo(ctxt);
    }
    
    /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
    template <typename CONCRETE_MODEL> 
    void Geant4FSShowerModel<CONCRETE_MODEL>::constructField(Geant4DetectorConstructionContext* ctxt)   {
      this->Geant4FastSimShowerModel::constructField(ctxt);
    }

    /// Sensitive detector construction callback. Called at "ConstructSDandField()"
    template <typename CONCRETE_MODEL> 
    void Geant4FSShowerModel<CONCRETE_MODEL>::constructSensitives(Geant4DetectorConstructionContext* ctxt)   {
      this->Geant4FastSimShowerModel::constructSensitives(ctxt);
    }

    /// User callback to determine if the model is applicable for the particle type
    template <typename CONCRETE_MODEL> 
    bool Geant4FSShowerModel<CONCRETE_MODEL>::check_applicability(const G4ParticleDefinition& particle)   {
      return this->Geant4FastSimShowerModel::check_applicability(particle);
    }

    /// User callback to determine if the shower creation should be triggered
    template <typename CONCRETE_MODEL> 
    bool Geant4FSShowerModel<CONCRETE_MODEL>::check_trigger(const G4FastTrack& track)   {
      return this->Geant4FastSimShowerModel::check_trigger(track);
    }

    /// User callback to model the particle/energy shower
    template <typename CONCRETE_MODEL> 
    void Geant4FSShowerModel<CONCRETE_MODEL>::modelShower(const G4FastTrack& track, G4FastStep& step)   {
      return this->Geant4FastSimShowerModel::modelShower(track, step);
    }
  }     /* End namespace sim   */
}       /* End namespace dd4hep */

#include "G4Version.hh"
#if G4VERSION_NUMBER > 1070
#include "G4FastSimHitMaker.hh"
#else
class G4FastSimHitMaker  {  public:  void make(const G4FastHit&, const G4FastTrack&)  { } };
#endif

#endif  // DDG4_GEANT4FASTSIMSHOWERMODEL_INL_H
