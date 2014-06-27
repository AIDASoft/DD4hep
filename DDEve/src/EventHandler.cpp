// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/EventHandler.h"
#include "DDEve/SimulationHit.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Factories.h"

// C/C++ include files
#include <stdexcept>

using namespace std;
using namespace DD4hep;
using DD4hep::DDEve::SimulationHit;

ClassImp(EventHandler)


namespace DD4hep {
  template <typename T> vector<const T*> EventHandler::GetHits(const string& collection)  {
    vector<T*> data;
    return data;
  }
  template<> vector<const SimulationHit*> EventHandler::GetHits(const string& collection);
}
template <typename T> static void _delete(void* p)  { T* t = (T*)p; deletePtr(t); }

template <typename T, typename Q> void fill_result(const T& src, Q& result)   {
  result.reserve(src.size());
  for(typename T::const_iterator i=src.begin(); i!=src.end(); ++i)
    result.push_back(&(*i));
}

template<> vector<const SimulationHit*> EventHandler::GetHits(const string& collection)  {
  typedef std::vector<const SimulationHit*> PTR_TYPE;
  typedef std::vector<SimulationHit> TYPE;
  PTR_TYPE result;
  EventCollections::key_type key = make_pair(&typeid(TYPE),collection);
  EventCollections::const_iterator ic = m_collections.find(key);
  if ( ic != m_collections.end() )   {
    const TYPE* source = (const TYPE*)((*ic).second.second);
    fill_result<TYPE,PTR_TYPE>(*source, result);
    return result;
  }
  const Data& event = data();
  for(Data::const_iterator i=event.begin(); i!=event.end(); ++i)  {
    for(std::vector<Collection>::const_iterator j=(*i).second.begin(); j!=(*i).second.end(); ++j)   {
      if ( (*j).first == collection )  {
	const std::vector<void*>* c = (*j).second;
	TYPE* pv = (TYPE*)ROOT::Reflex::PluginService::Create<void*>("DDEve_DDG4CollectionAccess",(const char*)c);
	m_collections[key] = make_pair(_delete<TYPE>,pv);
	fill_result<TYPE,PTR_TYPE>(*pv, result);
	return result;
      }
    }
  }
  return result;
}

/// Standard constructor
EventHandler::EventHandler() : m_hasEvent(false) {
}

/// Default destructor
EventHandler::~EventHandler()   {
  m_subscriptions.clear();
  ClearCache();
}

/// Clear all event related data caches
void EventHandler::ClearCache()     {
  for(EventCollections::const_iterator ic = m_collections.begin(); ic != m_collections.end(); ++ic)   {
    void (*fcn)(void*) = (*ic).second.first;
    (*fcn)((*ic).second.second);
  }
  m_collections.clear();
}

/// Notfy all subscribers
void EventHandler::NotifySubscribers(void (EventConsumer::*pmf)(EventHandler*))   {
  for(Subscriptions::iterator i=m_subscriptions.begin(); i!=m_subscriptions.end();++i)
    ((*i)->*pmf)(this);
}

/// Subscribe to notification of new data present
void EventHandler::Subscribe(EventConsumer* consumer)  {
  m_subscriptions.insert(consumer);
}

/// Unsubscribe from notification of new data present
void EventHandler::Unsubscribe(EventConsumer* consumer)  {
  Subscriptions::iterator i=m_subscriptions.find(consumer);
  if ( i != m_subscriptions.end() ) m_subscriptions.erase(i);
}


ClassImp(EventConsumer)

/// Standard constructor
EventConsumer::EventConsumer()  {
}

/// Default destructor
EventConsumer::~EventConsumer()   {
}


