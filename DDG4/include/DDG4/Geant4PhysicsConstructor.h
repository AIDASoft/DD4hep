//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
//  \author   Markus Frank
//  \date     2016-01-19
//  \version  1.0
//
//==========================================================================
#ifndef DDG4_GEANT4PHYSICSCONSTRUCTOR_H 
#define DDG4_GEANT4PHYSICSCONSTRUCTOR_H 1


// Framework include files
#include "DDG4/Geant4PhysicsList.h"

// Geant4 include files
#include "G4ParticleTable.hh"
#include "G4VPhysicsConstructor.hh"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    /// Implementation base of a Geant4 physics constructor
    /**
     * Concrete base of a Geant4 physics constructor.
     * This class is only useful for constructors, which require 
     * initialization (and hence instantiation) by the user BEFORE
     * the G4 physics list is instantiated.
     *
     * The instantiation of the physics list is delayed, because
     * Geant4 does not like the G4VPhysicsConstructor constructor
     * to be called before a physics list is attached to the
     * G4RunManager. We overcome this problem through the delayed
     * instantiation of a helper object made availible to the client.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4PhysicsConstructor : public Geant4PhysicsList   {
    public:

      /// Helper class to be used by cleints, by NOT overloaded!
      /** 
       *  The concrete instance will be instantiated by the framework in due time!
       *  Clients NEVER call the constructor directly
       *
       *  \author  M.Frank
       *  \version 1.0
       *  \ingroup DD4HEP_SIMULATION
       */
      class Constructor : virtual public G4VPhysicsConstructor   {
      protected:
        /// Initializing constructor dealing with the callbacks
        Constructor();
        /// Default destructor
        virtual ~Constructor();
      public:
        /// Access to a fresh (resetted) instance of the particle table iterator
        virtual G4ParticleTable::G4PTblDicIterator* particleIterator()  const = 0;
      };

    protected:
      /// Physics type (Default: 0)
      int m_type;
    public:
      /// Standard constructor with initailization parameters
      Geant4PhysicsConstructor(Geant4Context* context, const std::string& nam);
      /// Default destructor
      virtual ~Geant4PhysicsConstructor();
      /// constructPhysics callback
      virtual void constructPhysics(G4VModularPhysicsList* physics);
      /// Callback to construct processes (uses the G4 particle table)
      virtual void constructProcess(Constructor& ctor);
      /// Callback to construct particles
      virtual void constructParticle(Constructor& ctor);
    };

  }    // End namespace sim
}      // End namespace dd4hep

#endif // DDG4_GEANT4PHYSICSCONSTRUCTOR_H
