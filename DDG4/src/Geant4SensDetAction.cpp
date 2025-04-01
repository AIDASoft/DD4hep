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

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/InstanceCount.h>

#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4Mapping.h>
#include <DDG4/Geant4StepHandler.h>
#include <DDG4/Geant4SensDetAction.h>
#include <DDG4/Geant4VolumeManager.h>
#include <DDG4/Geant4MonteCarloTruth.h>

// Geant4 include files
#include <G4Step.hh>
#include <G4SDManager.hh>
#include <G4VSensitiveDetector.hh>

// C/C++ include files
#include <stdexcept>

#include "G4OpticalParameters.hh"
#include "G4OpticalPhoton.hh"

#ifdef DD4HEP_USE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif

using namespace dd4hep::sim;

#if 0
namespace {
  Geant4ActionSD* _getSensitiveDetector(const std::string& name) {
    G4SDManager* mgr = G4SDManager::GetSDMpointer();
    G4VSensitiveDetector* sd = mgr->FindSensitiveDetector(name);
    if (0 == sd) {
      dd4hep::except("Geant4Sensitive", "DDG4: You requested to configure actions "
                     "for the sensitive detector %s,\nDDG4: which is not known to Geant4. "
                     "Are you sure you already converted the geometry?", name.c_str());
    }
    Geant4ActionSD* action_sd = dynamic_cast<Geant4ActionSD*>(sd);
    if (0 == action_sd) {
      throw dd4hep::except("Geant4Sensitive", "DDG4: You may only configure actions "
                           "for sensitive detectors of type Geant4ActionSD.\n"
                           "DDG4: The sensitive detector of %s is of type %s, which is incompatible.", name.c_str(),
                           typeName(typeid(*sd)).c_str());
    }
    return action_sd;
  }
}
#endif

/// Standard action constructor
Geant4ActionSD::Geant4ActionSD(const std::string& nam)
  : Geant4Action(0, nam) {
  InstanceCount::increment(this);
}

/// Default destructor
Geant4ActionSD::~Geant4ActionSD() {
  InstanceCount::decrement(this);
}

/// Standard constructor
Geant4Filter::Geant4Filter(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam) {
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

/// Filter action. Return true if hits should be processed
bool Geant4Filter::operator()(const Geant4FastSimSpot*) const {
  except("The filter action %s does not support the GFLASH/FastSim interface for Geant4.", c_name());
  return false;
}

/// Constructor. The detector element is identified by the name
Geant4Sensitive::Geant4Sensitive(Geant4Context* ctxt, const std::string& nam, DetElement det, Detector& det_ref)
  : Geant4Action(ctxt, nam), m_detDesc(det_ref), m_detector(det)
{
  InstanceCount::increment(this);
  if (!det.isValid()) {
    except("DDG4: Detector elemnt for %s is invalid.", nam.c_str());
  }
  declareProperty("HitCreationMode", m_hitCreationMode = SIMPLE_MODE);
  m_sequence     = context()->kernel().sensitiveAction(m_detector.name());
  m_sensitive    = m_detDesc.sensitiveDetector(det.name());
  m_readout      = m_sensitive.readout();
  m_segmentation = m_readout.segmentation();
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
  except("Attempt to add invalid sensitive filter!");
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4Sensitive::adoptFilter_front(Geant4Action* action)   {
  Geant4Filter* filter = dynamic_cast<Geant4Filter*>(action);
  adopt_front(filter);
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4Sensitive::adopt_front(Geant4Filter* filter) {
  if (filter) {
    filter->addRef();
    m_filters.add_front(filter);
    return;
  }
  except("Attempt to add invalid sensitive filter!");
}

/// Callback before hit processing starts. Invoke all filters.
bool Geant4Sensitive::accept(const G4Step* step) const {
  bool (Geant4Filter::*filter)(const G4Step*) const = &Geant4Filter::operator();
  bool result = m_filters.filter(filter, step);
  return result;
}

/// GFLASH/FastSim interface: Callback before hit processing starts. Invoke all filters.
bool Geant4Sensitive::accept(const Geant4FastSimSpot* spot) const {
  bool (Geant4Filter::*filter)(const Geant4FastSimSpot*) const = &Geant4Filter::operator();
  bool result = m_filters.filter(filter, spot);
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
  except("DDG4: The sensitive detector for action %s was not properly configured.", name().c_str());
  throw std::runtime_error("Geant4Sensitive::detector");
}

/// Access to the hosting sequence
Geant4SensDetActionSequence& Geant4Sensitive::sequence() const {
  return *m_sequence;
}

/// Access the detector desciption object
dd4hep::Detector& Geant4Sensitive::detectorDescription() const {
  return m_detDesc;
}

/// Access HitCollection container names
const std::string& Geant4Sensitive::hitCollectionName(std::size_t which) const {
  return sequence().hitCollectionName(which);
}

/// Retrieve the hits collection associated with this detector by its serial number
Geant4HitCollection* Geant4Sensitive::collection(std::size_t which) {
  return sequence().collection(which);
}

/// Retrieve the hits collection associated with this detector by its collection identifier
Geant4HitCollection* Geant4Sensitive::collectionByID(std::size_t id) {
  return sequence().collectionByID(id);
}

/// Define collections created by this sensitivie action object
void Geant4Sensitive::defineCollections() {
}

/// Method invoked at the begining of each event.
void Geant4Sensitive::begin(G4HCofThisEvent* /* HCE */) {
}

/// Method invoked at the end of each event.
void Geant4Sensitive::end(G4HCofThisEvent* /* HCE */) {
}

/// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
bool Geant4Sensitive::process(const G4Step* /* step */, G4TouchableHistory* /* history */) {
  return false;
}

/// GFLASH/FastSim interface: Method for generating hit(s) using the information of the Geant4FastSimSpot object.
bool Geant4Sensitive::processFastSim(const Geant4FastSimSpot* /* spot */, G4TouchableHistory* /* history */) {
  except("The sensitive action %s does not support the GFLASH/FastSim interface for Geant4.", c_name());
  return false;
}

/// Method is invoked if the event abortion is occured.
void Geant4Sensitive::clear(G4HCofThisEvent* /* HCE */) {
}

/// Mark the track to be kept for MC truth propagation during hit processing
void Geant4Sensitive::mark(const G4Track* track) const  {
  Geant4MonteCarloTruth* truth = context()->event().extension<Geant4MonteCarloTruth>(false);
  if ( truth ) truth->mark(track);
}

/// Mark the track of this step to be kept for MC truth propagation during hit processing
void Geant4Sensitive::mark(const G4Step* step) const  {
  Geant4MonteCarloTruth* truth = context()->event().extension<Geant4MonteCarloTruth>(false);
  if ( truth ) truth->mark(step);
}

/// Returns the volumeID of the sensitive volume corresponding to the step
long long int Geant4Sensitive::volumeID(const G4Step* step) {
  Geant4StepHandler stepH(step);
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  VolumeID id = volMgr.volumeID(stepH.preTouchable());
  return id;
}

/// Returns the volumeID of the sensitive volume corresponding to the touchable history
long long int Geant4Sensitive::volumeID(const G4VTouchable* touchable) {
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  VolumeID id = volMgr.volumeID(touchable);
  return id;
}

/// Returns the cellID(volumeID+local coordinate encoding) of the sensitive volume corresponding to the step
long long int Geant4Sensitive::cellID(const G4Step* step) {
  Geant4StepHandler h(step);
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  bool UsePostStepOnly = G4OpticalParameters::Instance() && G4OpticalParameters::Instance()->GetBoundaryInvokeSD() && (step->GetTrack()->GetDefinition() == G4OpticalPhoton::Definition());
  VolumeID volID = volMgr.volumeID(UsePostStepOnly? h.postTouchable() : h.preTouchable());
  if ( m_segmentation.isValid() )  {
    std::exception_ptr eptr;
    G4ThreeVector global = UsePostStepOnly? h.postPosG4() : 0.5 * (h.prePosG4()+h.postPosG4());
    G4ThreeVector local  = UsePostStepOnly? h.postTouchable()->GetHistory()->GetTopTransform().TransformPoint(global) :
                                            h.preTouchable()->GetHistory()->GetTopTransform().TransformPoint(global);
    Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    try  {
      VolumeID cID = m_segmentation.cellID(loc, glob, volID);
      return cID;
    }
    catch(const std::exception& e)   {
      eptr = std::current_exception();
      error("cellID: failed to access segmentation for VolumeID: %016lX [%ld]  [%s]", volID, volID, e.what());
      error("....... G4-local:   (%f, %f, %f) G4-global:   (%f, %f, %f)",
	    local.x(), local.y(), local.z(), global.x(), global.y(), global.z());
      error("....... TGeo-local: (%f, %f, %f) TGeo-global: (%f, %f, %f)",
	    loc.x(), loc.y(), loc.z(), glob.x(), glob.y(), glob.z());
      error("....... Pre-step:  %s  SD: %s", h.volName(h.pre), h.sdName(h.pre).c_str());
      if ( h.post )
        error("....... Post-step: %s  SD: %s", h.volName(h.post), h.sdName(h.post).c_str());
      std::rethrow_exception(std::move(eptr));
    }
  }
  return volID;
}

/// Returns the cellID(volumeID+local coordinate encoding) of the sensitive volume corresponding to the touchable history
long long int Geant4Sensitive::cellID(const G4VTouchable* touchable, const G4ThreeVector& global) {
  Geant4VolumeManager volMgr = Geant4Mapping::instance().volumeManager();
  VolumeID volID = volMgr.volumeID(touchable);
  if ( m_segmentation.isValid() )  {
    std::exception_ptr eptr;
    G4ThreeVector local  = touchable->GetHistory()->GetTopTransform().TransformPoint(global);
    Position loc (local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    try  {
      VolumeID cID = m_segmentation.cellID(loc, glob, volID);
      return cID;
    }
    catch(const std::exception& e)   {
      auto* pvol = touchable->GetVolume();
      auto* vol = pvol->GetLogicalVolume();
      auto* sd = vol->GetSensitiveDetector();
      eptr = std::current_exception();
      error("cellID: failed to access segmentation for VolumeID: %016lX [%ld]  [%s]", volID, volID, e.what());
      error("....... G4-local:   (%f, %f, %f) G4-global:   (%f, %f, %f)",
	    local.x(), local.y(), local.z(), global.x(), global.y(), global.z());
      error("....... TGeo-local: (%f, %f, %f) TGeo-global: (%f, %f, %f)",
	    loc.x(), loc.y(), loc.z(), glob.x(), glob.y(), glob.z());
      error("....... Touchable:  %s  SD: %s", vol->GetName().c_str(), sd ? sd->GetName().c_str() : "???");
      std::rethrow_exception(std::move(eptr));
    }
  }
  return volID;
}

/// Standard constructor
Geant4SensDetActionSequence::Geant4SensDetActionSequence(Geant4Context* ctxt, const std::string& nam)
  : Geant4Action(ctxt, nam), m_hce(0), m_detector(0)
{
  m_needsControl = true;
  context()->sensitiveActions().insert(name(), this);
  /// Update the sensitive detector type, so that the proper instance is created
  m_sensitive = context()->detectorDescription().sensitiveDetector(nam);
  m_sensitiveType = m_sensitive.type();
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

/// Set or update client context
void Geant4SensDetActionSequence::updateContext(Geant4Context* ctxt)    {
  m_context = ctxt;
  m_actors.updateContext(ctxt);
  m_filters.updateContext(ctxt);
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
  except("Attempt to add invalid sensitive actor!");
}

/// Add an actor responding to all callbacks. Sequence takes ownership.
void Geant4SensDetActionSequence::adopt(Geant4Filter* filter) {
  if (filter) {
    filter->addRef();
    m_filters.add(filter);
    return;
  }
  except("Attempt to add invalid sensitive filter!");
}

/// Initialize the usage of a hit collection. Returns the collection identifier
std::size_t Geant4SensDetActionSequence::defineCollection(Geant4Sensitive* owner, const std::string& collection_name, create_t func) {
  m_collections.emplace_back(collection_name, make_pair(owner,func));
  return m_collections.size() - 1;
}

/// Called at construction time of the sensitive detector to declare all hit collections
std::size_t Geant4SensDetActionSequence::Geant4SensDetActionSequence::defineCollections(Geant4ActionSD* sens_det) {
  std::size_t count = 0;
  m_detector = sens_det;
  m_actors(&Geant4Sensitive::setDetector, sens_det);
  m_actors(&Geant4Sensitive::defineCollections);
  for (HitCollections::const_iterator i = m_collections.begin(); i != m_collections.end(); ++i) {
    sens_det->defineCollection((*i).first);
    ++count;
  }
  return count;
}

/// Access HitCollection container names
const std::string& Geant4SensDetActionSequence::hitCollectionName(std::size_t which) const {
  if (which < m_collections.size()) {
    return m_collections[which].first;
  }
  static std::string blank = "";
  except("The collection name index for subdetector %s is out of range!", c_name());
  return blank;
}

/// Retrieve the hits collection associated with this detector by its serial number
Geant4HitCollection* Geant4SensDetActionSequence::collection(std::size_t which) const {
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
Geant4HitCollection* Geant4SensDetActionSequence::collectionByID(std::size_t id) const {
  Geant4HitCollection* c = (Geant4HitCollection*) m_hce->GetHC(id);
  if (c)
    return c;
  except("The collection index for subdetector %s is wrong!", c_name());
  return 0;
}

/// Callback before hit processing starts. Invoke all filters.
bool Geant4SensDetActionSequence::accept(const G4Step* step) const {
  bool (Geant4Filter::*filter)(const G4Step*) const = &Geant4Filter::operator();
  bool result = m_filters.filter(filter, step);
  return result;
}

/// Callback before hit processing starts. Invoke all filters.
bool Geant4SensDetActionSequence::accept(const Geant4FastSimSpot* spot) const {
  bool (Geant4Filter::*filter)(const Geant4FastSimSpot*) const = &Geant4Filter::operator();
  bool result = m_filters.filter(filter, spot);
  return result;
}

/// G4VSensitiveDetector interface: Method for generating hit(s) using the information of G4Step object.
bool Geant4SensDetActionSequence::process(const G4Step* step, G4TouchableHistory* history) {
  bool result = false;
  for (Geant4Sensitive* sensitive : m_actors)  {
    if ( sensitive->accept(step) )
      result |= sensitive->process(step, history);
  }
  m_process(step, history);
  return result;
}

/// GFLASH/FastSim interface: Method for generating hit(s) using the information of the Geant4FastSimSpot object.
bool Geant4SensDetActionSequence::processFastSim(const Geant4FastSimSpot* spot, G4TouchableHistory* history)  {
  bool result = false;
  for (Geant4Sensitive* sensitive : m_actors)  {
    if ( sensitive->accept(spot) )
      result |= sensitive->processFastSim(spot, history);
  }
  m_process(spot, history);
  return result;
}

/** G4VSensitiveDetector interface: Method invoked at the begining of each event.
 *  The hits collection(s) created by this sensitive detector must
 *  be set to the G4HCofThisEvent object at one of these two methods.
 */
void Geant4SensDetActionSequence::begin(G4HCofThisEvent* hce) {
  m_hce = hce;
  for (std::size_t count = 0; count < m_collections.size(); ++count) {
    const HitCollection& cr = m_collections[count];
    Geant4HitCollection* col = (*cr.second.second)(name(), cr.first, cr.second.first);
    int id = m_detector->GetCollectionID(count);
    m_hce->AddHitsCollection(id, col);
  }
  m_actors(&Geant4Sensitive::begin, m_hce);
  m_begin (m_hce);
}

/// G4VSensitiveDetector interface: Method invoked at the end of each event.
void Geant4SensDetActionSequence::end(G4HCofThisEvent* hce) {
  m_end(hce);
  m_actors(&Geant4Sensitive::end, hce);
  // G4HCofThisEvent must be availible until end-event. m_hce = 0;
}

/// G4VSensitiveDetector interface: Method invoked if the event was aborted.
/** Hits collections created but not being set to G4HCofThisEvent
 *  at the event should be deleted.
 *  Collection(s) which have already set to G4HCofThisEvent
 *  will be deleted automatically.
 */
void Geant4SensDetActionSequence::clear() {
  m_clear (m_hce);
  m_actors(&Geant4Sensitive::clear, m_hce);
}

/// Default destructor
Geant4SensDetSequences::~Geant4SensDetSequences() {
  detail::releaseObjects(m_sequences);
  m_sequences.clear();
}

/// Access sequence member by name
Geant4SensDetActionSequence* Geant4SensDetSequences::operator[](const std::string& nam) const {
  std::string n = "SD_Seq_" + nam;
  Members::const_iterator i = m_sequences.find(n);
  if (i != m_sequences.end())
    return (*i).second;
  except("Attempt to access undefined SensDetActionSequence: %s ", nam.c_str());
  return nullptr;
}

/// Access sequence member by name
Geant4SensDetActionSequence* Geant4SensDetSequences::find(const std::string& name) const {
  std::string nam = "SD_Seq_" + name;
  Members::const_iterator i = m_sequences.find(nam);
  if (i != m_sequences.end())
    return (*i).second;
  return 0;
}

/// Insert sequence member
void Geant4SensDetSequences::insert(const std::string& name, Geant4SensDetActionSequence* seq) {
  if (seq) {
    std::string nam = "SD_Seq_" + name;
    seq->addRef();
    m_sequences[nam] = seq;
    return;
  }
  except("Attempt to add invalid sensitive sequence with name:%s", name.c_str());
}

/// Clear the sequence list
void Geant4SensDetSequences::clear()   {
  m_sequences.clear();
}
