// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4TestActions.h"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4Track.hh"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep::Simulation;
using namespace DD4hep::Simulation::Test;

namespace {
  struct TestHit {
    TestHit() {
    }
    virtual ~TestHit() {
    }
  };
}

#define PRINT info


/// Standard constructor
Geant4TestBase::Geant4TestBase(Geant4Action* a, const std::string& typ)
    : m_type(typ) {
  a->declareProperty("Property_int", m_value1 = 0);
  a->declareProperty("Property_double", m_value2 = 0e0);
  a->declareProperty("Property_string", m_value3);
  InstanceCount::increment(this);
}
/// Default destructor
Geant4TestBase::~Geant4TestBase() {
  InstanceCount::decrement(this);
}

/// Standard constructor with initializing arguments
Geant4TestGeneratorAction::Geant4TestGeneratorAction(Geant4Context* c, const std::string& n)
  : Geant4GeneratorAction(c, n), Geant4TestBase(this, "Geant4TestGeneratorAction") {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TestGeneratorAction::~Geant4TestGeneratorAction() {
  InstanceCount::decrement(this);
}

/// Callback to generate primary particles
void Geant4TestGeneratorAction::operator()(G4Event* evt)  {
  PRINT(name(), "%s> calling Geant4TestGeneratorAction(event_id=%d Context: run=%p evt=%p)", 
	m_type.c_str(), evt->GetEventID(), &context()->run(), &context()->event());
}

/// Standard constructor with initializing arguments
Geant4TestRunAction::Geant4TestRunAction(Geant4Context* c, const std::string& n)
  : Geant4RunAction(c, n), Geant4TestBase(this, "Geant4TestRunAction") {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TestRunAction::~Geant4TestRunAction() {
  InstanceCount::decrement(this);
}

/// begin-of-run callback
void Geant4TestRunAction::begin(const G4Run* run) {
  PRINT(name(), "%s> calling begin(run_id=%d,num_event=%d Context:%p)", m_type.c_str(), run->GetRunID(),
	run->GetNumberOfEventToBeProcessed(), &context()->run());
}

/// End-of-run callback
void Geant4TestRunAction::end(const G4Run* run) {
  PRINT(name(), "%s> calling end(run_id=%d, num_event=%d Context:%p)", 
	m_type.c_str(), run->GetRunID(), run->GetNumberOfEvent(), &context()->run());
}

/// begin-of-event callback
void Geant4TestRunAction::beginEvent(const G4Event* evt) {
  PRINT(name(), "%s> calling beginEvent(event_id=%d Context: run=%p evt=%p)", 
	m_type.c_str(), evt->GetEventID(), &context()->run(), &context()->event());
}

/// End-of-event callback
void Geant4TestRunAction::endEvent(const G4Event* evt) {
  PRINT(name(), "%s> calling endEvent(event_id=%d Context: run=%p evt=%p)", 
	m_type.c_str(), evt->GetEventID(), &context()->run(), &context()->event());
}

/// Standard constructor with initializing arguments
Geant4TestEventAction::Geant4TestEventAction(Geant4Context* c, const std::string& n)
    : Geant4EventAction(c, n), Geant4TestBase(this, "Geant4TestEventAction") {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TestEventAction::~Geant4TestEventAction() {
  InstanceCount::decrement(this);
}
/// begin-of-event callback
void Geant4TestEventAction::begin(const G4Event* evt) {
  PRINT(name(), "%s> calling begin(event_id=%d Context:  run=%p (%d) evt=%p (%d))", 
	m_type.c_str(), evt->GetEventID(), 
	&context()->run(), context()->run().run().GetRunID(),
	&context()->event(), context()->event().event().GetEventID());
}

/// End-of-event callback
void Geant4TestEventAction::end(const G4Event* evt) {
  PRINT(name(), "%s> calling end(event_id=%d Context:  run=%p (%d) evt=%p (%d))", 
	m_type.c_str(), evt->GetEventID(), &context()->run(), &context()->event(),
	&context()->run(), context()->run().run().GetRunID(),
	&context()->event(), context()->event().event().GetEventID());
}

/// begin-of-run callback
void Geant4TestEventAction::beginRun(const G4Run* run) {
  PRINT(name(), "%s> calling beginRun(run_id=%d,num_event=%d Context:%p)",
	   m_type.c_str(), run->GetRunID(),
	   run->GetNumberOfEventToBeProcessed(), &context()->run());
}

/// End-of-run callback
void Geant4TestEventAction::endRun(const G4Run* run) {
  PRINT(name(), "%s> calling endRun(run_id=%d, num_event=%d Context:%p)", 
	   m_type.c_str(), run->GetRunID(),
	   run->GetNumberOfEvent(), &context()->run());
}

/// Standard constructor with initializing arguments
Geant4TestTrackAction::Geant4TestTrackAction(Geant4Context* c, const std::string& n)
    : Geant4TrackingAction(c, n), Geant4TestBase(this, "Geant4TestTrackAction") {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TestTrackAction::~Geant4TestTrackAction() {
  InstanceCount::decrement(this);
}
/// Begin-of-tracking callback
void Geant4TestTrackAction::begin(const G4Track* trk) {
  PRINT(name(), "%s> calling begin(track=%d, parent=%d, position=(%f,%f,%f) Context: run=%p evt=%p)", 
	m_type.c_str(), trk->GetTrackID(),
	trk->GetParentID(), trk->GetPosition().x(), trk->GetPosition().y(), trk->GetPosition().z(), 
	&context()->run(), &context()->event());
}

/// End-of-tracking callback
void Geant4TestTrackAction::end(const G4Track* trk) {
  PRINT(name(), "%s> calling end(track=%d, parent=%d, position=(%f,%f,%f) Context: run=%p evt=%p)", 
	m_type.c_str(), trk->GetTrackID(),
	trk->GetParentID(), trk->GetPosition().x(), trk->GetPosition().y(), trk->GetPosition().z(), 
	&context()->run(), &context()->event());
}

/// Standard constructor with initializing arguments
Geant4TestStepAction::Geant4TestStepAction(Geant4Context* c, const std::string& n)
    : Geant4SteppingAction(c, n), Geant4TestBase(this, "Geant4TestStepAction") {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4TestStepAction::~Geant4TestStepAction() {
  InstanceCount::decrement(this);
}
/// User stepping callback
void Geant4TestStepAction::operator()(const G4Step*, G4SteppingManager*) {
  PRINT(name(), "%s> calling operator()", m_type.c_str());
}

/// Standard constructor with initializing arguments
Geant4TestSensitive::Geant4TestSensitive(Geant4Context* c, const std::string& n, DetElement det, LCDD& lcdd)
    : Geant4Sensitive(c, n, det, lcdd), Geant4TestBase(this, "Geant4TestSensitive") {
  InstanceCount::increment(this);
  m_collectionID = defineCollection < TestHit > (n);
  PRINT(name(), "%s> Collection ID is %d", m_type.c_str(), int(m_collectionID));
}

/// Default destructor
Geant4TestSensitive::~Geant4TestSensitive() {
  InstanceCount::decrement(this);
}

/// Begin-of-tracking callback
void Geant4TestSensitive::begin(G4HCofThisEvent* hce) {
  Geant4HitCollection* c = collectionByID(m_collectionID);
  PRINT(name(), "%s> calling begin(num_coll=%d, coll=%s Context: run=%p evt=%p)", 
	   m_type.c_str(), hce->GetNumberOfCollections(),
	c ? c->GetName().c_str() : "None", &context()->run(), &context()->event());
}

/// End-of-tracking callback
void Geant4TestSensitive::end(G4HCofThisEvent* hce) {
  Geant4HitCollection* c = collection(m_collectionID);
  PRINT(name(), "%s> calling end(num_coll=%d, coll=%s Context: run=%p evt=%p)", 
	m_type.c_str(), hce->GetNumberOfCollections(),
	c ? c->GetName().c_str() : "None", &context()->run(), &context()->event());
}

/// Method for generating hit(s) using the information of G4Step object.
bool Geant4TestSensitive::process(G4Step* step, G4TouchableHistory*) {
  Geant4HitCollection* c = collection(m_collectionID);
  PRINT(name(), "%s> calling process(track=%d, dE=%f, dT=%f len=%f, First,last in Vol=(%c,%c), coll=%s Context: run=%p evt=%p)",
	m_type.c_str(), step->GetTrack()->GetTrackID(), 
	   step->GetTotalEnergyDeposit(), step->GetDeltaTime(),
	   step->GetStepLength(), step->IsFirstStepInVolume() ? 'Y' : 'N', 
	   step->IsLastStepInVolume() ? 'Y' : 'N',
	c ? c->GetName().c_str() : "None", &context()->run(), &context()->event());
  return true;
}
