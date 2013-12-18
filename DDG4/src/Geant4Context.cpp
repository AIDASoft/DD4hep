// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#include <algorithm>
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Kernel.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Default constructor
Geant4Context::Geant4Context(Geant4Kernel* kernel)
    : m_kernel(kernel) {
}

/// Default destructor
Geant4Context::~Geant4Context() {
}

/// Access to detector description
Geometry::LCDD& Geant4Context::lcdd() const {
  return m_kernel->lcdd();
}

/// Create a user trajectory
G4VTrajectory* Geant4Context::createTrajectory(const G4Track* /* track */) const {
  string err = DD4hep::format("Geant4Kernel", "createTrajectory: Purely virtual method. requires overloading!");
  DD4hep::printout(DD4hep::FATAL, "Geant4Kernel", "createTrajectory: Purely virtual method. requires overloading!");
  throw runtime_error(err);
}

/// Access the tracking manager
G4TrackingManager* Geant4Context::trackMgr() const {
  return m_kernel->trackMgr();
}

/// Access to the main run action sequence from the kernel object
Geant4RunActionSequence& Geant4Context::runAction() const {
  return m_kernel->runAction();
}

/// Access to the main event action sequence from the kernel object
Geant4EventActionSequence& Geant4Context::eventAction() const {
  return m_kernel->eventAction();
}

/// Access to the main stepping action sequence from the kernel object
Geant4SteppingActionSequence& Geant4Context::steppingAction() const {
  return m_kernel->steppingAction();
}

/// Access to the main tracking action sequence from the kernel object
Geant4TrackingActionSequence& Geant4Context::trackingAction() const {
  return m_kernel->trackingAction();
}

/// Access to the main stacking action sequence from the kernel object
Geant4StackingActionSequence& Geant4Context::stackingAction() const {
  return m_kernel->stackingAction();
}

/// Access to the main generator action sequence from the kernel object
Geant4GeneratorActionSequence& Geant4Context::generatorAction() const {
  return m_kernel->generatorAction();
}

/// Access to the main generator action sequence from the kernel object
Geant4SensDetSequences& Geant4Context::sensitiveActions() const {
  return m_kernel->sensitiveActions();
}

/// Access to the Track Manager from the kernel object
Geant4MonteCarloTruth& Geant4Context::mcTruthMgr() const   {
  return m_kernel->mcTruthMgr();
}

/// Access to the MC record manager from the kernel object
Geant4MonteCarloRecordManager& Geant4Context::mcRecordMgr() const   {
  return m_kernel->mcRecordMgr();
}

