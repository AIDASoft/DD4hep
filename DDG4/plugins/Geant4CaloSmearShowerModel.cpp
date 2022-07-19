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
//
// Please note:
//
// These shower parametrizations is a generalized form of the Geant4 example:
// <geant4-source-dir>/examples/extended/parameterisations/Par02
//
//==========================================================================

// Framework include files
#include <DDG4/Geant4FastSimShowerModel.inl.h>
#include <DDG4/Geant4FastSimSpot.h>
#include <DDG4/Geant4Random.h>

// Geant4 include files
#include "G4SystemOfUnits.hh"
#include "G4FastStep.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    ///===================================================================================================
    ///
    ///  Par02 electromagnetic shower model (e+, e-)
    ///
    ///===================================================================================================

    /// Configuration structure for the fast simulation shower model Geant4FSShowerModel<par02_em_model>
    class calo_smear_model  {
    public:
      G4FastSimHitMaker hitMaker          { };
      double            StocasticEnergyResolution { -1e0 };
      double            ConstantEnergyResolution  { -1e0 };
      double            NoiseEnergyResolution     { -1e0 };

      double resolution(double momentum)   const    {
	double res = -1e0;
	double mom = momentum/CLHEP::GeV;
	if ( this->StocasticEnergyResolution > 0 && 
	     this->ConstantEnergyResolution  > 0 &&
	     this->NoiseEnergyResolution > 0 )    {
	  res = std::sqrt(std::pow( this->StocasticEnergyResolution / std::sqrt( mom ), 2 ) +  // stochastic
			  std::pow( this->ConstantEnergyResolution, 2 ) +                      // constant
			  std::pow( this->NoiseEnergyResolution / mom, 2 ) );                  // noise
	}
	else if ( this->StocasticEnergyResolution > 0 &&
		  this->ConstantEnergyResolution > 0 )    {
	  res = std::sqrt(std::pow( this->StocasticEnergyResolution / std::sqrt( mom ), 2 ) +  // stochastic
			  std::pow( this->ConstantEnergyResolution, 2 ) );                     // constant
	}
	else if ( this->StocasticEnergyResolution > 0 )    {
	  res = this->StocasticEnergyResolution / std::sqrt( mom );                            // stochastic
	}
	else if ( this->ConstantEnergyResolution > 0 )    {
	  res = this->ConstantEnergyResolution;                                                // constant
	}
	return res;
      }

      double smearEnergy(double mom)   const  {
	double resolution = this->resolution(mom);
	double smeared    = mom;
	if ( resolution > 0e0 )  {
	  for( smeared = -1e0; smeared < 0e0; ) {  // Ensure that the resulting value is not negative
	    smeared = mom * Geant4Random::instance()->gauss(1e0, resolution);
	  }
	}
	return smeared;
      }
    };
    
    /// Declare optional properties from embedded structure
    template <> 
    void Geant4FSShowerModel<calo_smear_model>::initialize()     {
      declareProperty("StocasticResolution", locals.StocasticEnergyResolution );
      declareProperty("ConstantResolution", locals.ConstantEnergyResolution );
      declareProperty("NoiseResolution", locals.NoiseEnergyResolution );
    }

    /// Sensitive detector construction callback. Called at "ConstructSDandField()"
    template <>
    void Geant4FSShowerModel<calo_smear_model>::constructSensitives(Geant4DetectorConstructionContext* ctxt)   {
      this->Geant4FastSimShowerModel::constructSensitives(ctxt);
    }

    /// User callback to model the particle/energy shower
    template <> 
    void Geant4FSShowerModel<calo_smear_model>::modelShower(const G4FastTrack& track, G4FastStep& step)   {
      auto* primary = track.GetPrimaryTrack();
      // Kill the parameterised particle:
      this->killParticle(step, primary->GetKineticEnergy(), 0e0);
      //-----------------------------------------------------
      G4FastHit hit;
      Geant4FastSimSpot spot(&hit, &track);
      G4ThreeVector     position = spot.trackPosition();
      double            deposit  = spot.kineticEnergy();

      hit.SetPosition(spot.trackPosition());
      // Consider only primary tracks and smear according to the parametrized resolution
      // ELSE: simply set the value of the (initial) energy of the particle is deposited in the step
      if ( !spot.primary->GetParentID() ) {
	deposit = locals.smearEnergy(deposit);
      }
      hit.SetEnergy(deposit);
      step.ProposeTotalEnergyDeposited(deposit);
      this->locals.hitMaker.make(hit, track);
    }

    typedef Geant4FSShowerModel<calo_smear_model> Geant4CaloSmearShowerModel;
  }
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION_NS(dd4hep::sim,Geant4CaloSmearShowerModel)

