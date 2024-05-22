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
// These shower parametrizations come direct from the Geant4 example:
// <geant4-source-dir>/examples/extended/parameterisations/Par01
//
// From the README:
// 	      o Par01EMShowerModel which provides a crude
//		parameterisation for e+/e-/gamma. This model
//		is bound to the EM calorimeter.
//
//	      o Par01PionShowerModel: an even more crude
//		parameterisation for pi+/pi-. This model
//		is bound to a ghost volume.
//
//==========================================================================

// Framework include files
#include <DDG4/Geant4FastSimShowerModel.inl.h>
#include <DDG4/Geant4FastSimSpot.h>
#include <DDG4/Geant4Random.h>

// Geant4 include files
#include <G4Gamma.hh>
#include <G4SystemOfUnits.hh>

// C/C++ include files


/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    ///===================================================================================================
    ///
    ///  Par01 electromagnetic shower model (e+, e-)
    ///
    ///===================================================================================================

    /// Configuration structure for the fast simulation shower model Geant4FSShowerModel<par01_em_model>
    class par01_em_model  {
    public:
      G4FastSimHitMaker hitMaker          { };
      std::string       materialName      { };
      G4Material*       material          { nullptr };
      double            criticalEnergyRef { 800*MeV };
      double            criticalEnergy    { 800*MeV };
    };
    
    /// Declare optional properties from embedded structure
    template <> 
    void Geant4FSShowerModel<par01_em_model>::initialize()     {
      declareProperty("Material",       this->locals.materialName);
      declareProperty("CriticalEnergy", this->locals.criticalEnergyRef);
      this->m_applicablePartNames.emplace_back("e+");
      this->m_applicablePartNames.emplace_back("e-");
    }

    /// Sensitive detector construction callback. Called at "ConstructSDandField()"
    template <>
    void Geant4FSShowerModel<par01_em_model>::constructSensitives(Geant4DetectorConstructionContext* ctxt)   {
      locals.material       = this->getMaterial(this->locals.materialName);
      locals.criticalEnergy = this->locals.criticalEnergyRef * (this->locals.material->GetZ() + 1.2);
      this->Geant4FastSimShowerModel::constructSensitives(ctxt);
    }

    /// User callback to model the particle/energy shower
    template <> 
    void Geant4FSShowerModel<par01_em_model>::modelShower(const G4FastTrack& track, G4FastStep& step)   {
      auto* primary = track.GetPrimaryTrack();
      // Kill the parameterised particle:
      this->killParticle(step, primary->GetKineticEnergy(), 0e0);
      //-----------------------------------------------------
      // split into "energy spots" energy according to the shower shape:
      // See Par01EMShowerModel::Explode(track);
      //-----------------------------------------------------
      G4FastHit hit;
      Geant4FastSimSpot spot(&hit, &track);

      // Reduced quantities:   -- critical energy in material:
      G4double Ec      = this->locals.criticalEnergy;
      G4double Energy  = spot.kineticEnergy();
      G4double y       = Energy/Ec;
      // compute value of parameter "a" of longitudinal profile, b assumed = 0.5
      G4double b = 0.5, C = -0.5;
      if (spot.trackDefinition() == G4Gamma::GammaDefinition()) C = 0.5;
      G4double tmax    = 1.0 * (std::log(y) + C);
      G4double a       = 1.0 + b*tmax;
      // radiation length
      G4double X0      = this->locals.material->GetRadlen();
      // Moliere radius:
      G4double Es      = 21*MeV;
      G4double Rm      = X0*Es/Ec;
      // We shoot 100 spots of energy:
      G4int    nSpots  = 100;
      G4double deposit = Energy/double(nSpots);

      // axis of the shower, in global reference frame:
      G4ThreeVector xShower, yShower, zShower;
      zShower = spot.particleLocalDirection();
      xShower = zShower.orthogonal();
      yShower = zShower.cross(xShower);
      // starting point of the shower:
      Geant4Random* rndm    = Geant4Random::instance();
      G4ThreeVector sShower = spot.particleLocalPosition();
      for (int i = 0; i < nSpots; i++)    {
	// Longitudinal profile: -- shoot z according to Gamma distribution:
	G4double bt  = rndm->gamma(a, 1e0);
	G4double t   = bt/b;                       // t : reduced quantity = z/X0:
	G4double z   = t*X0;
	// transverse profile: we set 90% of energy in one Rm, the rest between 1 and 3.5 Rm:
	G4double xr  = rndm->uniform(0e0,1e0);
	G4double phi = rndm->uniform(0e0,twopi);
	G4double r;
	if (xr < 0.9) r = xr/0.9*Rm;
	else r = ((xr - 0.9)/0.1*2.5 + 1.0)*Rm;
	// build the position:
	G4ThreeVector position = sShower + z*zShower + r*std::cos(phi)*xShower + r*std::sin(phi)*yShower;
	/// Process spot and call sensitive detector
	hit.SetPosition(position);
	hit.SetEnergy(deposit);
	this->locals.hitMaker.make(hit, track);
      }
    }

    ///===================================================================================================
    ///
    ///  Par01 Pion shower model (pi+, pi-)
    ///
    ///===================================================================================================
    
    /// Configuration structure for the fast simulation shower model Geant4FSShowerModel<par01_pion_model>
    class par01_pion_model  {
    public:
      G4FastSimHitMaker hitMaker { };
    };
    
    /// Declare optional properties from embedded structure
    template <> 
    void Geant4FSShowerModel<par01_pion_model>::initialize()     {
      this->m_applicablePartNames.emplace_back("pi+");
      this->m_applicablePartNames.emplace_back("pi-");
    }

    /// User callback to determine if the shower creation should be triggered
    template <> 
    bool Geant4FSShowerModel<par01_pion_model>::check_trigger(const G4FastTrack& /* track */)   {
      return true;
    }

    /// User callback to model the particle/energy shower
    template <> 
    void Geant4FSShowerModel<par01_pion_model>::modelShower(const G4FastTrack& track, G4FastStep& step)   {
      auto* primary = track.GetPrimaryTrack();
      // Kill the parameterised particle:
      this->killParticle(step, primary->GetKineticEnergy(), 0e0);

      //-----------------------------------------------------
      // Non-physical shower generated: exp along z and transverse.
      //-----------------------------------------------------
      // center of the shower, we put at the middle of the ghost:
      G4ThreeVector pos = track.GetPrimaryTrackLocalPosition();
      G4ThreeVector dir = track.GetPrimaryTrackLocalDirection();
      G4double      distOut = track.GetEnvelopeSolid()->DistanceToOut(pos, dir);
      G4ThreeVector showerCenter = pos + (distOut/2.)*dir;

      showerCenter = track.GetInverseAffineTransformation()->TransformPoint(showerCenter);

      // axis of the shower, in global reference frame:
      G4ThreeVector zShower = primary->GetMomentumDirection();
      G4ThreeVector xShower = zShower.orthogonal();
      G4ThreeVector yShower = zShower.cross(xShower);

      // shoot the energy spots:
      G4double      Energy  = primary->GetKineticEnergy();
      G4int         nSpot   = 50;
      G4double      deposit = Energy/double(nSpot);
      Geant4Random* rndm    = Geant4Random::instance();
      for (int i = 0; i < nSpot; i++)  {
	double z   = rndm->gauss(0, 20*cm);
	double r   = rndm->gauss(0, 10*cm);
	double phi = rndm->uniform(0e0, twopi);
	G4ThreeVector position = showerCenter + z*zShower + r*std::cos(phi)*xShower + r*std::sin(phi)*yShower;
	/// Process spot and call sensitive detector
	G4FastHit hit(position, deposit);
	this->locals.hitMaker.make(hit, track);
      }
    }

    typedef Geant4FSShowerModel<par01_em_model>   Geant4Par01EMShowerModel;
    typedef Geant4FSShowerModel<par01_pion_model> Geant4Par01PionShowerModel;
  }
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION_NS(dd4hep::sim,Geant4Par01EMShowerModel)
DECLARE_GEANT4ACTION_NS(dd4hep::sim,Geant4Par01PionShowerModel)
