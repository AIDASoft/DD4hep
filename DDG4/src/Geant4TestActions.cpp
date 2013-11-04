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
#include "DDG4/Factories.h"
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
    TestHit() {}
    virtual ~TestHit() {}
  };
}

/// Standard constructor
Geant4TestBase::Geant4TestBase(Geant4Action* a,const std::string& typ)
: m_type(typ)  {
  a->declareProperty("Property_int",   m_value1=0);	
  a->declareProperty("Property_double",m_value2=0e0);	
  a->declareProperty("Property_string",m_value3);	
  InstanceCount::increment(this);
}
/// Default destructor
Geant4TestBase::~Geant4TestBase() {
  InstanceCount::decrement(this);
}

Geant4TestRunAction::Geant4TestRunAction(Geant4Context* c, const std::string& n) 
: Geant4RunAction(c,n), Geant4TestBase(this,"Geant4TestRunAction") {
  InstanceCount::increment(this);
}
Geant4TestRunAction::~Geant4TestRunAction() {
  InstanceCount::decrement(this);
}
/// begin-of-run callback
void  Geant4TestRunAction::begin(const G4Run* run)  {
  printout(INFO,name(),"%s> calling begin(run_id=%d,num_event=%d)",
	   m_type.c_str(),run->GetRunID(),run->GetNumberOfEventToBeProcessed());
}
/// End-of-run callback
void  Geant4TestRunAction::end  (const G4Run* run) {
  printout(INFO,name(),"%s> calling end(run_id=%d, num_event=%d)",
	   m_type.c_str(),run->GetRunID(),run->GetNumberOfEvent());
}
/// begin-of-event callback
void  Geant4TestRunAction::beginEvent(const G4Event* evt)  {
  printout(INFO,name(),"%s> calling beginEvent(event_id=%d)",m_type.c_str(),evt->GetEventID());
}
/// End-of-event callback
void  Geant4TestRunAction::endEvent  (const G4Event* evt) {
  printout(INFO,name(),"%s> calling endEvent(event_id=%d)",m_type.c_str(), evt->GetEventID());
}

Geant4TestEventAction::Geant4TestEventAction(Geant4Context* c, const std::string& n) 
: Geant4EventAction(c,n), Geant4TestBase(this,"Geant4TestEventAction") {
  InstanceCount::increment(this);
}
Geant4TestEventAction::~Geant4TestEventAction() {
  InstanceCount::decrement(this);
}
/// begin-of-event callback
void  Geant4TestEventAction::begin(const G4Event* evt) {
  printout(INFO,name(),"%s> calling begin(event_id=%d)",m_type.c_str(),evt->GetEventID());
}

/// End-of-event callback
void  Geant4TestEventAction::end  (const G4Event* evt) {
  printout(INFO,name(),"%s> calling end(event_id=%d)",m_type.c_str(),evt->GetEventID());
}

/// begin-of-run callback
void  Geant4TestEventAction::beginRun(const G4Run* run)  {
  printout(INFO,name(),"%s> calling beginRun(run_id=%d,num_event=%d)",
	   m_type.c_str(),run->GetRunID(),run->GetNumberOfEventToBeProcessed());
}

/// End-of-run callback
void  Geant4TestEventAction::endRun  (const G4Run* run) {
  printout(INFO,name(),"%s> calling endRun(run_id=%d, num_event=%d)",
	   m_type.c_str(),run->GetRunID(),run->GetNumberOfEvent());
}

Geant4TestTrackAction::Geant4TestTrackAction(Geant4Context* c, const std::string& n) 
: Geant4TrackingAction(c,n), Geant4TestBase(this,"Geant4TestTrackAction") {
  InstanceCount::increment(this);
}
Geant4TestTrackAction::~Geant4TestTrackAction() {
  InstanceCount::decrement(this);
}
/// Begin-of-tracking callback
void Geant4TestTrackAction::begin(const G4Track* trk) {
  printout(INFO,name(),"%s> calling begin(track=%d, parent=%d, position=(%f,%f,%f))",
	   m_type.c_str(),trk->GetTrackID(),trk->GetParentID(),
	   trk->GetPosition().x(),trk->GetPosition().y(),trk->GetPosition().z());
}

/// End-of-tracking callback
void Geant4TestTrackAction::end(const G4Track* trk) {
  printout(INFO,name(),"%s> calling end(track=%d, parent=%d, position=(%f,%f,%f))",
	   m_type.c_str(),trk->GetTrackID(),trk->GetParentID(),
	   trk->GetPosition().x(),trk->GetPosition().y(),trk->GetPosition().z());
}

Geant4TestStepAction::Geant4TestStepAction(Geant4Context* c, const std::string& n) 
: Geant4SteppingAction(c,n), Geant4TestBase(this,"Geant4TestStepAction") {
  InstanceCount::increment(this);
}
Geant4TestStepAction::~Geant4TestStepAction() {
  InstanceCount::decrement(this);
}
/// User stepping callback
void Geant4TestStepAction::operator()(const G4Step* , G4SteppingManager* ) {
  printout(INFO,name(),"%s> calling operator()",m_type.c_str());
}

Geant4TestSensitive::Geant4TestSensitive(Geant4Context* c, const std::string& n, DetElement det, LCDD& lcdd) 
: Geant4Sensitive(c,n,det,lcdd), Geant4TestBase(this,"Geant4TestSensitive") {
  InstanceCount::increment(this);
  m_collectionID = defineCollection<TestHit>(n);
  printout(INFO,name(),"%s> Collection ID is %d",m_type.c_str(),int(m_collectionID));
}
Geant4TestSensitive::~Geant4TestSensitive() {
  InstanceCount::decrement(this);
}

/// Begin-of-tracking callback
void Geant4TestSensitive::begin(G4HCofThisEvent* hce) {
  Geant4HitCollection* c = collectionByID(m_collectionID);
  printout(INFO,name(),"%s> calling begin(num_coll=%d, coll=%s)",
	   m_type.c_str(),hce->GetNumberOfCollections(),
	   c ? c->GetName().c_str() : "None");
}

/// End-of-tracking callback
void Geant4TestSensitive::end(G4HCofThisEvent* hce) {
  Geant4HitCollection* c = collection(m_collectionID);
  printout(INFO,name(),"%s> calling end(num_coll=%d, coll=%s)",
	   m_type.c_str(),hce->GetNumberOfCollections(),
	   c ? c->GetName().c_str() : "None");
}

/// Method for generating hit(s) using the information of G4Step object.
bool Geant4TestSensitive::process(G4Step* step,G4TouchableHistory* )   {
  Geant4HitCollection* c = collection(m_collectionID);
  printout(INFO,name(),"%s> calling process(track=%d, dE=%f, dT=%f len=%f, First,last in Vol=(%c,%c), coll=%s)",
	   m_type.c_str(),step->GetTrack()->GetTrackID(),
	   step->GetTotalEnergyDeposit(), step->GetDeltaTime(), step->GetStepLength(),
	   step->IsFirstStepInVolume() ? 'Y' : 'N',
	   step->IsLastStepInVolume() ? 'Y' : 'N',
	   c ? c->GetName().c_str() : "None");
  return true;
}
namespace DD4hep {  namespace Simulation   {
    //using namespace Test;
    using namespace DD4hep::Simulation::Test;
    typedef Geant4TestSensitive Geant4TestSensitiveTracker;
    typedef Geant4TestSensitive Geant4TestSensitiveCalorimeter;
  }}

DECLARE_GEANT4ACTION(Geant4TestRunAction)
DECLARE_GEANT4ACTION(Geant4TestEventAction)
DECLARE_GEANT4ACTION(Geant4TestStepAction)
DECLARE_GEANT4ACTION(Geant4TestTrackAction)
//DECLARE_GEANT4ACTION(Geant4TestStackingAction)
//DECLARE_GEANT4ACTION(Geant4TestGeneratorAction)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitive)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitiveTracker)
DECLARE_GEANT4SENSITIVE(Geant4TestSensitiveCalorimeter)
