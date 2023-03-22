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
#ifndef DDG4_GEANT4PARTICLEINFORMATION_H
#define DDG4_GEANT4PARTICLEINFORMATION_H

/// Framework include files
#include <DDG4/Geant4Particle.h>

/// Geant4 include files
#include <G4VUserTrackInformation.hh>

/// C/C++ include files
#include <string>
#include <memory>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Wrapper to store user information in a G4Track.
    /** Wrapper to store user information in a G4Track.
     *  The data of type ParticleExtension is moved from the G4Track to the 
     *  Geant4Particle in the Geant4ParticleHandler if present.
     *  This automatically make the G4Track instance persistent as a 
     *  Geant4Particle.
     *  Hence: Be careful to not assign the entity by default!
     *
     *  The data in the subclass of ParticleExtension defined by the user
     *  requires a dictionary to be stored to ROOT.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4ParticleInformation : public G4VUserTrackInformation {
      /// Keep track of the user data
      std::unique_ptr<ParticleExtension> extension;

    public:
      /// Default Constructor
      Geant4ParticleInformation() = default;
      /// Initializing Constructor
      template <typename EXTENSION_TYPE>
      Geant4ParticleInformation(EXTENSION_TYPE* data);
      /// Initializing Constructor
      template <typename EXTENSION_TYPE>
      Geant4ParticleInformation(std::unique_ptr<EXTENSION_TYPE>&& data);
      /// Move constructor
      Geant4ParticleInformation(Geant4ParticleInformation&& copy) = default;
      /// Disable copy constructor
      Geant4ParticleInformation(const Geant4ParticleInformation& copy) = delete;
      /// Move assignemtn operator
      Geant4ParticleInformation& operator=(Geant4ParticleInformation&& copy) = default;
      /// Disable copy assignment
      Geant4ParticleInformation& operator=(const Geant4ParticleInformation& copy) = delete;
      /// Default destructor
      virtual ~Geant4ParticleInformation() = default;
      
      /// Attach information
      void set(ParticleExtension* data)   {
	this->extension.reset(data);
      }
      /// Attach information
      template <typename EXTENSION_TYPE> void set(std::unique_ptr<EXTENSION_TYPE>&& data)   {
	this->extension = std::move(data);
      }
      template <typename EXTENSION_TYPE> EXTENSION_TYPE* get()   {
	return dynamic_cast<EXTENSION_TYPE*>(this->extension.get());
      }
      ParticleExtension* get()   {
	return this->extension.get();
      }
      ParticleExtension* release()   {
	return this->extension.release();
      }
    };

    /// Initializing Constructor
    template <typename EXTENSION_TYPE> inline
    Geant4ParticleInformation::Geant4ParticleInformation(std::unique_ptr<EXTENSION_TYPE>&& data)
      : extension(std::move(data))
    {
    }

    /// Initializing Constructor
    template <typename EXTENSION_TYPE> inline
    Geant4ParticleInformation::Geant4ParticleInformation(EXTENSION_TYPE* data)
      : extension(data)
    {
    }

  }    // End namespace sim
}      // End namespace dd4hep
#endif /* DDG4_GEANT4PARTICLEINFORMATION_H */
