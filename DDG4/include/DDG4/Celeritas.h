#ifndef Celeritas_h
#define Celeritas_h 1

#include <accel/SimpleOffload.hh>
#include <G4EmStandardPhysics.hh>

namespace celeritas
{
    class LocalTransporter;
    struct SetupOptions;
    class SharedParams;

    class EMPhysicsConstructor final : public G4EmStandardPhysics
    {
    public:
        using G4EmStandardPhysics::G4EmStandardPhysics;

        void ConstructProcess() override;
    };
}

// Global shared setup options
celeritas::SetupOptions& CelerSetupOptions();
// Shared data and GPU setup
celeritas::SharedParams& CelerSharedParams();
// Thread-local transporter
celeritas::LocalTransporter& CelerLocalTransporter();
// Thread-local offload
celeritas::SimpleOffload& CelerSimpleOffload();

#endif
