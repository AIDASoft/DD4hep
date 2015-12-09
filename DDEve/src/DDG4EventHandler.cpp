// $Id: $
//==========================================================================
//  AIDA Detector description implementation for LCD
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
#include "DDEve/DDG4EventHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Factories.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;

ClassImp(DDG4EventHandler)
namespace {
  union FCN  {
    FCN(void* p) { ptr = p; }
    DDG4EventHandler::HitAccessor_t hits;
    DDG4EventHandler::ParticleAccessor_t particles;
    void* ptr;
  };
  /// Factory entry point
  void* _create(const char*)  {
    EventHandler* h = new DDG4EventHandler();
    return h;
  }
}
using namespace DD4hep::Geometry;
DECLARE_CONSTRUCTOR(DDEve_DDG4EventHandler,_create)

/// Standard constructor
DDG4EventHandler::DDG4EventHandler() : EventHandler(), m_file(0,0), m_entry(-1) {
  void* ptr = PluginService::Create<void*>("DDEve_DDG4HitAccess",(const char*)"");
  if ( 0 == ptr )   {
    throw runtime_error("FATAL: Failed to access function pointer from factory DDEve_DDG4HitAccess");
  }
  m_simhitConverter = FCN(ptr).hits;
  ptr = PluginService::Create<void*>("DDEve_DDG4ParticleAccess",(const char*)"");
  if ( 0 == ptr )   {
    throw runtime_error("FATAL: Failed to access function pointer from factory DDEve_DDG4ParticleAccess");
  }
  m_particleConverter = FCN(ptr).particles;
}

/// Default destructor
DDG4EventHandler::~DDG4EventHandler()   {
  if ( m_file.first )  {
    m_file.first->Close();
    delete m_file.first;
  }
  m_file.first = 0;
  m_file.second = 0;
}

/// Load the next event
bool DDG4EventHandler::NextEvent()   {
  return ReadEvent(++m_entry) > 0;
}

/// Load the previous event
bool DDG4EventHandler::PreviousEvent()   {
  return ReadEvent(--m_entry) > 0;
}

/// Goto a specified event in the file
bool DDG4EventHandler::GotoEvent(long event_number)   {
  return ReadEvent(m_entry = event_number) > 0;
}

/// Access the number of events on the current input data source (-1 if no data source connected)
long DDG4EventHandler::numEvents() const   {
  if ( hasFile() )  {
    return m_file.second->GetEntries();
  }
  return -1;
}

/// Access the data source name
std::string DDG4EventHandler::datasourceName() const   {
  if ( hasFile() )  {
    return m_file.first->GetName();
  }
  return "UNKNOWN";
}

/// Access to the collection type by name
EventHandler::CollectionType DDG4EventHandler::collectionType(const std::string& collection) const {
  Branches::const_iterator i = m_branches.find(collection);
  if ( i != m_branches.end() )   {
    const char* cl = (*i).second.first->GetClassName();
    if ( ::strstr(cl,"Simulation::Geant4Calorimeter::Hit") )  return CALO_HIT_COLLECTION;
    else if ( ::strstr(cl,"Simulation::Geant4Tracker::Hit") ) return TRACKER_HIT_COLLECTION;
    else if ( ::strstr(cl,"Simulation::Geant4Particle") )     return PARTICLE_COLLECTION;
    // These are OLD types. Eventually remove these lines.....
    else if ( ::strstr(cl,"Simulation::SimpleCalorimeter::Hit") ) return CALO_HIT_COLLECTION;
    else if ( ::strstr(cl,"Simulation::SimpleTracker::Hit") )     return TRACKER_HIT_COLLECTION;
    else if ( ::strstr(cl,"Simulation::Particle") )               return PARTICLE_COLLECTION;
  }
  return NO_COLLECTION;
}

/// Call functor on hit collection
size_t DDG4EventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor)   {
  typedef std::vector<void*> _P;
  Branches::const_iterator i = m_branches.find(collection);
  if ( i != m_branches.end() )   {
    const _P* data_ptr = (_P*)(*i).second.second;
    if ( data_ptr )  {
      DDEveHit hit;
      actor.setSize(data_ptr->size());
      for(_P::const_iterator j=data_ptr->begin(); j!=data_ptr->end(); ++j)   {
        if ( (*m_simhitConverter)(*j,&hit) )    {
          actor(hit);
        }
      }
      return data_ptr->size();
    }
  }
  return 0;
}

/// Loop over collection and extract particle data
size_t DDG4EventHandler::collectionLoop(const std::string& collection, DDEveParticleActor& actor)    {
  typedef std::vector<void*> _P;
  Branches::const_iterator i = m_branches.find(collection);
  if ( i != m_branches.end() )   {
    const _P* data_ptr = (_P*)(*i).second.second;
    if ( data_ptr )  {
      DDEveParticle part;
      actor.setSize(data_ptr->size());
      for(_P::const_iterator j=data_ptr->begin(); j!=data_ptr->end(); ++j)   {
        if ( (*m_particleConverter)(*j,&part) )    {
          actor(part);
        }
      }
      return data_ptr->size();
    }
  }
  return 0;
}

/// Load the specified event
Int_t DDG4EventHandler::ReadEvent(Long64_t event_number)   {
  m_data.clear();
  m_hasEvent = false;
  if ( hasFile() )  {
    if ( event_number >= m_file.second->GetEntries() )  {
      event_number = m_file.second->GetEntries()-1;
      printout(ERROR,"DDG4EventHandler","+++ ReadEvent: Cannot read across End-of-file! Reading last event:%d.",event_number);
    }
    else if ( event_number < 0 )  {
      event_number = 0;
      printout(ERROR,"DDG4EventHandler","+++ nextEvent: Cannot read across Start-of-file! Reading first event:%d.",event_number);
    }

    Int_t nbytes = m_file.second->GetEntry(event_number);
    if ( nbytes >= 0 )   {
      printout(ERROR,"DDG4EventHandler","+++ ReadEvent: Read %d bytes of event data for entry:%d",nbytes,event_number);
      for(Branches::const_iterator i=m_branches.begin(); i != m_branches.end(); ++i)  {
        TBranch* b = (*i).second.first;
        std::vector<void*>* ptr_data = *(std::vector<void*>**)b->GetAddress();
        m_data[b->GetClassName()].push_back(make_pair(b->GetName(),ptr_data->size()));
      }
      m_hasEvent = true;
      return nbytes;
    }
    printout(ERROR,"DDG4EventHandler","+++ ReadEvent: Cannot read event data for entry:%d",event_number);
    throw runtime_error("+++ EventHandler::readEvent: Failed to read event");
  }
  throw runtime_error("+++ EventHandler::readEvent: No file open!");
}

/// Open new data file
bool DDG4EventHandler::Open(const std::string&, const std::string& name)   {
  if ( m_file.first ) m_file.first->Close();
  m_hasFile = false;
  m_hasEvent = false;
  TFile* f = TFile::Open(name.c_str());
  if ( f && !f->IsZombie() )  {
    m_file.first = f;
    TTree* t = (TTree*)f->Get("EVENT");
    if ( t )   {
      TObjArray* br = t->GetListOfBranches();
      m_file.second = t;
      m_entry = -1;
      m_branches.clear();
      for(Int_t i=0; i<br->GetSize(); ++i)  {
        TBranch* b = (TBranch*)br->At(i);
        if ( !b ) continue;
        m_branches[b->GetName()] = make_pair(b,(void*)0);
        printout(INFO,"DDG4EventHandler::open","+++ Branch %s has %ld entries.",b->GetName(),b->GetEntries());
      }
      for(Int_t i=0; i<br->GetSize(); ++i)  {
        TBranch* b = (TBranch*)br->At(i);
        if ( !b ) continue;
        b->SetAddress(&m_branches[b->GetName()].second);
      }
      m_hasFile = true;
      return true;
    }
    throw runtime_error("+++ Failed to access tree EVENT in ROOT file:"+name);
  }
  throw runtime_error("+++ Failed to open ROOT file:"+name);
}
