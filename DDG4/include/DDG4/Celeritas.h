#ifndef Celeritas_h
#define Celeritas_h 1

#include <accel/SimpleOffload.hh>
#include <G4EmStandardPhysics.hh>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep  {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim  {

    class EMPhysicsConstructor final : public G4EmStandardPhysics
    {
    public:
        using G4EmStandardPhysics::G4EmStandardPhysics;

        void ConstructProcess() override;
    };
    
  } /* End namespace sim */  
} /* End namespace dd4hep*/

// Global shared setup options
celeritas::SetupOptions& CelerSetupOptions();
// Shared data and GPU setup
celeritas::SharedParams& CelerSharedParams();
// Thread-local transporter
celeritas::LocalTransporter& CelerLocalTransporter();
// Thread-local offload
celeritas::SimpleOffload& CelerSimpleOffload();

#endif
