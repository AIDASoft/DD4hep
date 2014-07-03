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

using namespace DD4hep;

ClassImp(EventHandler)
ClassImp(EventConsumer)

/// Standard constructor
EventHandler::EventHandler() : m_hasFile(false), m_hasEvent(false) {
}

/// Default destructor
EventHandler::~EventHandler()   {
  m_subscriptions.clear();
  ClearCache();
}

/// Clear all event related data caches
void EventHandler::ClearCache()     {
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

/// Standard constructor
EventConsumer::EventConsumer()  {
}

/// Default destructor
EventConsumer::~EventConsumer()   {
}


