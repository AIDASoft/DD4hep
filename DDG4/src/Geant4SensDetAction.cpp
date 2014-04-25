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
#include "DD4hep/Primitives.h"
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4Kernel.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4StepHandler.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Geant4MonteCarloTruth.h"

#include <G4Step.hh>
#include <G4SDManager.hh>
#include <G4VSensitiveDetector.hh>

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

namespace {
  Geant4ActionSD* _getSensitiveDetector(const string& name) {
    G4SDManager* mgr = G4SDManager::GetSDMpointer();
    G4VSensitiveDetector* sd = mgr->FindSensitiveDetector(name);
    if (0 == sd) {
      throw runtime_error(format("Geant4Sensitive", "DDG4: You requested to configure actions "
          "for the sensitive detector %s,\nDDG4: which is not known to Geant4. "
          "Are you sure you already converted the geometry?", name.c_str()));
    }
    Geant4ActionSD* action_sd = dynamic_cast<Geant4ActionSD*>(sd);
    if (0 == action_sd) {
      throw runtime_error(
          format("Geant4Sensitive", "DDG4: You may only configure actions "
              "for sensitive detectors of type Geant4ActionSD.\n"
              "DDG4: The sensitive detector of %s is of type %s, which is incompatible.", name.c_str(),
              typeName(typeid(*sd)).c_str()));
    }
    return action_sd;
  }
}

/// Standard action constructor
Geant4ActionSD::Geant4ActionSD(const std::string& name)
    : Geant4Action(0, name) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ActionSD::~Geant4ActionSD() {
  InstanceCount::decrement(this);
}

/// Standard constructor
Geant4Filter::Geant4Filter(Geant4Context* context, const std::string& name)
    : Geant4Action(context, name) {
  InstanceCount::increment(this);
}

/// Standard destructor
Geant4Filter::~Geant4Filter() {
  InstanceCount::decrement(this);
}

/// Filter action. Return true if hits should be processed
bool Geant4Filter::operator()(const G4Step*) const {
  return true;
}

/// Constructor. The detector element is identified by the name
Geant4Sensitive::Geant4Sensitive(Geant4Context* ctxt, const string& name, DetElement det, LCDD& lcdd)
    : Geant4Action(ctxt, name), m_sensitiveDetector(0), m_sequence(0), m_lcdd(lcdd), m_detector(det), m_sensitive(), m_readout() {
  InstanceCount::increment(this);
  if (!det.isValid()) {
    throw runtime_error(format("Geant4Sensitive", "DDG4: Detector elemnt for %s is invalid.", name.c_str()));
  }
  m_sequence  = ctxt->kernel().sensitiveAction(m_detector.name());
  m_sensitive = lcdd.sensitiveDetector(det.name());
  m_readout   = m_sensitive.readout();
}

/// Standard destructor
Geant4Sensitive::~Geant4Sensitive() {
  m_filters(&Geant4Filter::release);
  m_filters.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4Sensitive::adoptFilter(Geant4Action* action)   {
  Geant4Filter* filter = dynamic_cast<Geant4Filter*>(action);
  adopt(filter);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4Sensitive::adopt(Geant4Filter* filter) {
  if (filter) {
    filter->addRef();
    m_filters.add(filter);
    return;
  }
  throw runtime_error("Geant4SensDetActionSequence: Attempt to add invalid sensitive filter!");
}

/// Callback before hit processing starts. Invoke all filters.
bool Geant4Sensitive::accept(const G4Step* step) const {
  bool result = m_filters.filter(&Geant4Filter::operator(), step);
  return result;
}

/// Access to the sensitive detector object
void Geant4Sensitive::setDetector(Geant4ActionSD* sens_det) {
  m_sensitiveDetector = sens_det;
}

/// Access to the sensitive detector object
Geant4ActionSD& Geant4Sensitive::detector() const {
  if (m_sensitiveDetector)
    return *m_sensitiveDetector;
  //m_sensitiveDetector = _getSensitiveDetector(m_detector.name());
  //if (  m_sensitiveDetector ) return *m_sensitiveDetector;
  throw runtime_error(format("Geant4Sensitive", "DDG4: The sensitive detector for action %s "
      "was not properly configured.", name().c_str()));
}

/// Access to the hosting sequence
Geant4SensDetActionSequence& Geant4Sensitive::sequence() const {
  return *m_sequence;
}

/// Access HitCollection container names
const string& Geant4Sensitive::hitCollectionName(size_t which) const {
  return sequence().hitCollectionName(which);
}

/// Retrieve the hits collection associated with this detector by its serial number
Geant4HitCollection* Geant4Sensitive::collection(size_t which) {
  return sequence().collection(which);
}

/// Retrieve the hits collection associated with this detector by its collection identifier
Geant4HitCollection* Geant4Sensitive::collectionByID(size_t id) {
  return sequence().collectionByID(id);
}

/// Method invoked at the begining of each event.
void Geant4Sensitive::begin(G4HCofThisEvent* /* HCE */) {
}

/// Method invoked at the end of each event.
void Geant4Sensitive::end(G4HCofThisEvent* /* HCE */) {
}

/// Method for generating hit(s) using the information of G4Step object.
bool Geant4Sensitive::process(G4Step* /* step */, G4TouchableHistory* /* history */) {
  return false;
}

/// Method is invoked if the event abortion is occured.
void Geant4Sensitive::clear(G4HCofThisEvent* /* HCE */) {
}

/// Mark the track to be kept for MC truth propagation during hit processing
void Geant4Sensitive::mark(const G4Track* track) const  {
  mcTruthMgr().mark(track);
}

/// Mark the track of this step to be kept for MC truth propagation during hit processing
void Geant4Sensitive::mark(const G4Step* step) const  {
  mcTruthMgr().mark(step);
}

/// Returns the volumeID of the sensitive volume corresponding to the step
long long int Geant4Sensitive::volumeID(G4Step* s) {
  Geant4StepHandler step(s);
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  VolumeID id = volMgr.volumeID(step.preTouchable());
  return id;
}

/// Standard constructor
Geant4SensDetActionSequence::Geant4SensDetActionSequence(Geant4Context* context, const string& nam)
    : Geant4Action(context, nam), m_hce(0) {
  m_needsControl = true;
  context->sensitiveActions().insert(name(), this);
  /// Update the sensitive detector type, so that the proper instance is created
  Geometry::SensitiveDetector sd = context->lcdd().sensitiveDetector(nam);
  sd.setType("Geant4SensDet");
  InstanceCount::increment(this);
}

/// Default destructor
Geant4SensDetActionSequence::~Geant4SensDetActionSequence() {
  m_filters(&Geant4Filter::release);
  m_actors(&Geant4Sensitive::release);
  m_filters.clear();
  m_actors.clear();
  InstanceCount::decrement(this);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4SensDetActionSequence::adoptFilter(Geant4Action* action)   {
  Geant4Filter* filter = dynamic_cast<Geant4Filter*>(action);
  adopt(filter);
}

/// Add an actor responding to all callbacks
void Geant4SensDetActionSequence::adopt(Geant4Sensitive* sensitive) {
  if (sensitive) {
    sensitive->addRef();
    m_actors.add(sensitive);
    return;
  }
  throw runtime_error("Geant4SensDetActionSequence: Attempt to add invalid sensitive actor!");
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4SensDetActionSequence::adopt(Geant4Filter* filter) {
  if (filter) {
    filter->addRef();
    m_filters.add(filter);
    return;
  }
  throw runtime_error("Geant4SensDetActionSequence: Attempt to add invalid sensitive filter!");
}

/// Initialize the usage of a hit collection. Returns the collection identifier
size_t Geant4SensDetActionSequence::defineCollection(const std::string& name, create_t func) {
  m_collections.push_back(make_pair(name, func));
  return m_collections.size() - 1;
}

/// Called at construction time of the sensitive detector to declare all hit collections
size_t Geant4SensDetActionSequence::Geant4SensDetActionSequence::defineCollections(Geant4ActionSD* sens_det) {
  size_t count = 0;
  m_detector = sens_det;
  m_actors(&Geant4Sensitive::setDetector, sens_det);
  for (HitCollections::const_iterator i = m_collections.begin(); i != m_collections.end(); ++i) {
    sens_det->defineCollection((*i).first);
    ++count;
  }
  return count;
}

/// Access HitCollection container names
const std::string& Geant4SensDetActionSequence::hitCollectionName(size_t which) const {
  if (which < m_collections.size()) {
    return m_collections[which].first;
  }
  static string blank = "";
  except("The collection name index for subdetector %s is out of range!", c_name());
  return blank;
}

/// Retrieve the hits collection associated with this detector by its serial number
Geant4HitCollection* Geant4SensDetActionSequence::collection(size_t which) const {
  if (which < m_collections.size()) {
    int hc_id = m_detector->GetCollectionID(which);
    Geant4HitCollection* c = (Geant4HitCollection*) m_hce->GetHC(hc_id);
    if (c)
      return c;
    except("The collection index for subdetector %s is wrong!", c_name());
  }
  except("The collection name index for subdetector %s is out of range!", c_name());
  return 0;
}

/// Retrieve the hits collection associated with this detector by its collection identifier
Geant4HitCollection* Geant4SensDetActionSequence::collectionByID(size_t id) const {
  Geant4HitCollection* c = (Geant4HitCollection*) m_hce->GetHC(id);
  if (c)
    return c;
  except("The collection index for subdetector %s is wrong!", c_name());
  return 0;
}

/// Callback before hit processing starts. Invoke all filters.
bool Geant4SensDetActionSequence::accept(const G4Step* step) const {
  bool result = m_filters.filter(&Geant4Filter::operator(), step);
  return result;
}

/// Function to process hits
bool Geant4SensDetActionSequence::process(G4Step* step, G4TouchableHistory* hist) {
  bool result = false;
  for (vector<Geant4Sensitive*>::iterator i = m_actors->begin(); i != m_actors->end(); ++i) {
    Geant4Sensitive* s = *i;
    if (s->accept(step))
      result |= s->process(step, hist);
  }
  m_process(step, hist);
  return result;
}

/** G4VSensitiveDetector interface: Method invoked at the begining of each event.
 *  The hits collection(s) created by this sensitive detector must
 *  be set to the G4HCofThisEvent object at one of these two methods.
 */
void Geant4SensDetActionSequence::begin(G4HCofThisEvent* hce) {
  m_hce = hce;
  for (size_t count = 0; count < m_collections.size(); ++count) {
    const std::pair<string, create_t>& cr = m_collections[count];
    G4VHitsCollection* c = (*cr.second)(name(), cr.first);
    int id = m_detector->GetCollectionID(count);
    m_hce->AddHitsCollection(id, c);
  }
  m_actors(&Geant4Sensitive::begin, m_hce);
  m_begin (m_hce);
}

/// G4VSensitiveDetector interface: Method invoked at the end of each event.
void Geant4SensDetActionSequence::end(G4HCofThisEvent* hce) {
  m_end(hce);
  m_actors(&Geant4Sensitive::end, hce);
  m_hce = 0;
}

/// G4VSensitiveDetector interface: Method invoked if the event was aborted.
/** Hits collections created but not beibg set to G4HCofThisEvent
 *  at the event should be deleted.
 *  Collection(s) which have already set to G4HCofThisEvent
 *  will be deleted automatically.
 */
void Geant4SensDetActionSequence::clear() {
  m_clear (m_hce);
  m_actors(&Geant4Sensitive::clear, m_hce);
}

/// Default constructor
Geant4SensDetSequences::Geant4SensDetSequences() {
}

/// Default destructor
Geant4SensDetSequences::~Geant4SensDetSequences() {
  for_each(m_sequences.begin(), m_sequences.end(), releaseObjects(m_sequences));
  m_sequences.clear();
}

/// Access sequence member by name
Geant4SensDetActionSequence* Geant4SensDetSequences::operator[](const string& name) const {
  string nam = "SD_Seq_" + name;
  Members::const_iterator i = m_sequences.find(nam);
  if (i != m_sequences.end())
    return (*i).second;
  throw runtime_error("Attempt to access undefined SensDetActionSequence!");
}

/// Access sequence member by name
Geant4SensDetActionSequence* Geant4SensDetSequences::find(const std::string& name) const {
  string nam = "SD_Seq_" + name;
  Members::const_iterator i = m_sequences.find(nam);
  if (i != m_sequences.end())
    return (*i).second;
  return 0;
}

/// Insert sequence member
void Geant4SensDetSequences::insert(const string& name, Geant4SensDetActionSequence* seq) {
  if (seq) {
    string nam = "SD_Seq_" + name;
    seq->addRef();
    m_sequences[nam] = seq;
    return;
  }
  throw runtime_error(format("Geant4SensDetSequences", "Attempt to add invalid sensitive "
      "sequence with name:%s", name.c_str()));
}

/// Clear the sequence list
void Geant4SensDetSequences::clear()   {
  m_sequences.clear();
}
