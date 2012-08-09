// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDG4/Geant4SensitiveDetector.h"
#include "DDG4/Geant4Hits.h"
#include "DD4hep/LCDD.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Simulation;

/// Constructor. The detector element is identified by the name
Geant4SensitiveDetector::Geant4SensitiveDetector(const string& name, LCDD& lcdd)
  : G4VSensitiveDetector(name), m_detector(), m_lcdd(lcdd), m_hce(0)
{
  m_sensitive = lcdd.sensitiveDetector(name);
  m_detector  = lcdd.detector(name);
  m_readout   = m_detector.readout();
}

/// Standard destructor
Geant4SensitiveDetector::~Geant4SensitiveDetector() {
}

/// Initialize the sensitive detector for the usage of a single hit collection
bool Geant4SensitiveDetector::defineCollections(const std::string& coll_name) {
  collectionName.insert(coll_name);
}

/// Initialize the sensitive detector for the usage of a multiple hit collection
bool Geant4SensitiveDetector::defineCollections(const HitCollectionNames& coll_names) {
  for(HitCollectionNames::const_iterator i=coll_names.begin(); i!=coll_names.end();++i)
    collectionName.insert(*i);
}


/// Access HitCollection container names
const string& Geant4SensitiveDetector::hitCollectionName(int which) const      { 
  if ( which >= collectionName.size() || which < 0 ) {
    throw runtime_error("The collection name index for subdetector "+name()+" is out of range!");
  }
  return collectionName[which];
}

/// Create single hits collection
Geant4SensitiveDetector::HitCollection* Geant4SensitiveDetector::createCollection(const std::string& coll_name)  const  {
  return new G4THitsCollection<Geant4Hit>(GetName(), coll_name);
}
namespace DD4hep { namespace Simulation {
template <> Geant4CalorimeterHit* 
Geant4SensitiveDetector::find<Geant4CalorimeterHit>(const HitCollection* c,const HitCompare<Geant4CalorimeterHit>& cmp)   {
  typedef std::vector<Geant4CalorimeterHit*> _V;
  const _V* v = (const _V*)c->GetVector();
  for(_V::const_iterator i=v->begin(); i !=v->end(); ++i) 
    if ( cmp(*i) ) return *i;
  return 0;
}
}}

/// Method invoked at the begining of each event. 
void Geant4SensitiveDetector::Initialize(G4HCofThisEvent* HCE) {
  int count = 0;
  m_hce = 0;
  for(G4CollectionNameVector::const_iterator i=collectionName.begin(); i!=collectionName.end();++i,++count) {
    G4VHitsCollection* c = createCollection(*i);
    HCE->AddHitsCollection(GetCollectionID(count),c);
  }
}

/// Method invoked at the end of each event. 
void Geant4SensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE) {
  m_hce = 0;
  // Eventuall print event summary
}

/// Method for generating hit(s) using the information of G4Step object.
G4bool Geant4SensitiveDetector::ProcessHits(G4Step* step,G4TouchableHistory* hist) {
  if ( step->GetTotalEnergyDeposit() > m_sensitive.energyCutoff() ) {
    if ( !Geant4Hit::isGeantino(step->GetTrack()) )   {
      return buildHits(step,hist);
    }
  }
  return false;
}

/// Retrieve the hits collection associated with this detector by its collection identifier
Geant4SensitiveDetector::HitCollection* Geant4SensitiveDetector::collectionByID(int id)    {
  HitCollection* hc = (HitCollection*)m_hce->GetHC(id);
  if ( hc ) return hc;
  throw runtime_error("The collection index for subdetector "+name()+" is wrong!");
}

/// Retrieve the hits collection associated with this detector by its serial number
Geant4SensitiveDetector::HitCollection* Geant4SensitiveDetector::collection(int which)    {
  if ( which < collectionName.size() && which < 0 ) {
    HitCollection* hc = (HitCollection*)m_hce->GetHC(GetCollectionID(which));
    if ( hc ) return hc;
    throw runtime_error("The collection index for subdetector "+name()+" is wrong!");
  }
  throw runtime_error("The collection name index for subdetector "+name()+" is out of range!");
}

/// Method is invoked if the event abortion is occured.
void Geant4SensitiveDetector::clear() {
}

//DECLARE_TRANSLATION(LCDD2Geant4,create_g4);
