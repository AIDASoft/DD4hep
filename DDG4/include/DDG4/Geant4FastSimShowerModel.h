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
#ifndef DDG4_GEANT4FASTSIMSHOWERMODEL_H
#define DDG4_GEANT4FASTSIMSHOWERMODEL_H

// Framework include files
#include <DDG4/Geant4DetectorConstruction.h>

// Geant4 include files
#include <G4FastSimulationPhysics.hh>

// C/C++ include files
#include <set>
#include <vector>
#include <memory>

/// Forward declarations
class G4FastStep;
class G4FastTrack;
class G4ParticleDefinition;
class G4VFastSimulationModel;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    /// Forard declarations
    class Geant4ShowerModelWrapper;
    
    /// Geant4 wrapper for the Geant4 fast simulation shower model
    /**
     *  Geant4 wrapper for the Geant4 fast simulation shower model
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4FastSimShowerModel : public Geant4DetectorConstruction    {
    protected:
      // typedef std::vector<std::pair<std::string, double> > ParticleConfig;
      typedef std::map<const G4ParticleDefinition*, double> ParticleCut;
      typedef std::map<std::string, std::string> ParticleConfig;
      typedef Geant4ShowerModelWrapper* Wrapper;
  
      /// Property: Region name to which this parametrization should be applied
      std::string    m_regionName     { "Region-name-not-specified"};
      /// Property: Particle names for which this parametrization is applicable
      std::vector<std::string> m_applicablePartNames  { };
      /// Property: Parametrisation control: Enable/disable fast simulation
      bool           m_enable         { false };
      /// Property: Defines step lenght
      double         m_stepX0         { 0.1 };
      /// Property: Set minimum kinetic energy to trigger parametrisation
      ParticleConfig m_eMin           { };
      /// Property: Set maximum kinetic energy to trigger parametrisation
      ParticleConfig m_eMax           { };
      /// Property: Set maximum kinetic energy for particles to be killed
      ParticleConfig m_eKill          { };
      /// Property: Set minmal kinetic energy for particles to trigger the model
      ParticleConfig m_eTriggerNames  { };

      /// Particle definitions for which this parametrization is applicable
      std::set<const G4ParticleDefinition*> m_applicableParticles  { };
      /// Particle cut to trigger model simulation indexed by G4ParticleDefinition
      ParticleCut    m_eTriggerCut    { };
      /// Reference to the G4 fast simulation model
      G4VFastSimulationModel* m_model { nullptr };
      /// Reference to the shower model
      Wrapper        m_wrapper        { nullptr };

    protected:
      /// Define standard assignments and constructors
      DDG4_DEFINE_ACTION_CONSTRUCTORS(Geant4FastSimShowerModel);

      /// Get parametrization material
      G4Material* getMaterial(const std::string& name)   const;
      /// Access particle definition from string
      G4ParticleDefinition* getParticleDefinition(const std::string& name)   const;
      /// Access the region from the detector description by name
      G4Region* getRegion(const std::string& nam)   const;
      /// Add shower model to region's fast simulation manager
      void addShowerModel(G4Region* region);
      /// Kill primary particle when creating the shower
      void killParticle(G4FastStep& step, double deposit, double step_length = 0e0);

    public:
      /// Standard constructor
      Geant4FastSimShowerModel(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4FastSimShowerModel();

      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt);
      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt);
      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt);

      /// User callback to determine if the model is applicable for the particle type
      virtual bool check_applicability(const G4ParticleDefinition& particle);
      /// User callback to determine if the shower creation should be triggered
      virtual bool check_trigger(const G4FastTrack& track);
      /// User callback to model the particle/energy shower
      virtual void modelShower(const G4FastTrack& track, G4FastStep& step);
    };


    /// Geant4 wrapper for the Geant4 fast simulation shower model
    /**
     *  Geant4 wrapper for the Geant4 fast simulation shower model
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    template <typename CONCRETE_MODEL>
    class Geant4FSShowerModel : public Geant4FastSimShowerModel    {

    public:
      /// Local parameter buffer to specialize caches, properties etc.
      CONCRETE_MODEL locals;

    public:
      /// Standard constructor
      Geant4FSShowerModel(Geant4Context* context, const std::string& nam);

      /// Default destructor
      virtual ~Geant4FSShowerModel();

      /// Declare optional properties from embedded structure
      void initialize();

      /// Geometry construction callback. Called at "Construct()"
      virtual void constructGeo(Geant4DetectorConstructionContext* ctxt)  override;

      /// Electromagnetic field construction callback. Called at "ConstructSDandField()"
      virtual void constructField(Geant4DetectorConstructionContext* ctxt)  override;

      /// Sensitive detector construction callback. Called at "ConstructSDandField()"
      virtual void constructSensitives(Geant4DetectorConstructionContext* ctxt)  override;

      /// User callback to determine if the model is applicable for the particle type
      /** Default implementation checks if the particle is registered in 'ApplicableParticles'
       */
      virtual bool check_applicability(const G4ParticleDefinition& particle)  override;

      /// User callback to determine if the shower creation should be triggered
      /** Default implementation checks if for all particles registered in 'Etrigger'
       *  the kinetic energy is bigger than the value.
       */
      virtual bool check_trigger(const G4FastTrack& track)  override;

      /// User callback to model the particle/energy shower
      virtual void modelShower(const G4FastTrack& track, G4FastStep& step)  override;
    };
  }     /* End namespace sim   */
}       /* End namespace dd4hep */
#endif  /* DDG4_GEANT4FASTSIMSHOWERMODEL_H  */
