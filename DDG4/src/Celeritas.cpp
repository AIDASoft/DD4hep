#include <DDG4/Celeritas.h>

#include <G4Threading.hh>
#include <accel/AlongStepFactory.hh>
#include <celeritas/field/UniformFieldData.hh>
#include <celeritas/io/ImportData.hh>
#include <accel/LocalTransporter.hh>
#include <accel/SetupOptions.hh>
#include <accel/SharedParams.hh>
#include <accel/TrackingManagerOffload.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>
#include <G4Gamma.hh>

#include <memory>


using namespace dd4hep::sim;

// Global shared setup options
celeritas::SetupOptions& CelerSetupOptions()
{
  static celeritas::SetupOptions options = [] {
    // Construct setup options the first time CelerSetupOptions is invoked
    celeritas::SetupOptions so;

    // Set along-step factory
    so.make_along_step = celeritas::UniformAlongStepFactory();
    // NOTE: these numbers are appropriate for CPU execution
    so.max_num_tracks = 1024;
    so.initializer_capacity = 1024 * 128;
    // Celeritas does not support EmStandard MSC physics above 100 MeV
    so.ignore_processes = {"CoulombScat"};

    // Use Celeritas "hit processor" to call back to Geant4 SDs.
    so.sd.enabled = false;

    // Only call back for nonzero energy depositions: this is currently a
    // global option for all detectors, so if any SDs extract data from tracks
    // with no local energy deposition over the step, it must be set to false.
    so.sd.ignore_zero_deposition = false;

    // Using the pre-step point, reconstruct the G4 touchable handle.
    so.sd.locate_touchable = true;

    // Pre-step time is used
    so.sd.pre.global_time = true;
    return so;
  }();
  return options;
}

// Shared data and GPU setup
celeritas::SharedParams& CelerSharedParams()
{
  static celeritas::SharedParams sp;
  return sp;
}

// Thread-local transporter
celeritas::LocalTransporter& CelerLocalTransporter()
{
  static G4ThreadLocal celeritas::LocalTransporter lt;
  return lt;
}

// Thread-local offload interface
celeritas::SimpleOffload& CelerSimpleOffload()
{
  static G4ThreadLocal celeritas::SimpleOffload so;
  return so;
}

void EMPhysicsConstructor::ConstructProcess()
{
    CELER_LOG_LOCAL(status) << "Setting up tracking manager offload";
    G4EmStandardPhysics::ConstructProcess();

    // Add Celeritas tracking manager to electron, positron, gamma.
    auto* celer_tracking = new celeritas::TrackingManagerOffload(
        &CelerSharedParams(), &CelerLocalTransporter());
    
    G4Electron::Definition()->SetTrackingManager(celer_tracking);
    G4Positron::Definition()->SetTrackingManager(celer_tracking);
    G4Gamma::Definition()->SetTrackingManager(celer_tracking);
}
