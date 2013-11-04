// $Id: Geant4Mapping.cpp 588 2013-06-03 11:41:35Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/ComponentUtils.h"
#include "G4Allocator.hh"

using namespace DD4hep::Simulation;

static G4Allocator<Geant4HitWrapper> HitWrapperAllocator;

Geant4HitWrapper::InvalidHit::~InvalidHit()  {
}

Geant4HitWrapper::HitManipulator::HitManipulator(const std::type_info& t, destroy_t d, cast_t c) : type(t), destroy(d), cast(c) 
{
}

void Geant4HitWrapper::HitManipulator::checkHitType(const std::type_info& test_type)  const  {
  if ( test_type != type )   {
    throw unrelated_type_error(test_type,type,
			       "This hit cannot be passed to this hit collection!");
  }
}

/// Default destructor
Geant4HitWrapper::~Geant4HitWrapper() {
  if ( m_data.first && m_data.second )  {
    (m_data.second->destroy)(m_data);
  }
}


/// Geant4 required object allocator
void* Geant4HitWrapper::operator new(size_t)   { 
  return HitWrapperAllocator.MallocSingle();    
}

/// Geat4 required object destroyer
void Geant4HitWrapper::operator delete(void *p) { 
  HitWrapperAllocator.FreeSingle((Geant4HitWrapper*)p); 
}

/// Pointer/Object release
void* Geant4HitWrapper::release()  {
  void* p = m_data.first;
  m_data.first = 0;
  m_data.second = manipulator<InvalidHit>();
  return p;
}

/// Pointer/Object release
Geant4HitWrapper::Wrapper Geant4HitWrapper::releaseData()  {
  Wrapper w = m_data;
  m_data.first = 0;
  m_data.second = manipulator<InvalidHit>();
  return w;
}

/// Default destructor
Geant4HitCollection::~Geant4HitCollection()  {
  m_hits.clear();
  InstanceCount::decrement(this);
}

/// Notification to increase the instance counter
void Geant4HitCollection::newInstance()   {
  InstanceCount::increment(this);
}

/// Find hit in a collection by comparison of attributes
void* Geant4HitCollection::findHit(const Compare& cmp)  const  {
  void* p = 0;
  for(WrappedHits::const_iterator i=m_hits.begin(); i !=m_hits.end(); ++i) 
    if ( (p=cmp(*i)) !=0 ) return p;
  return p;
}

