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
#ifndef DD4HEP_DDEVE_GENERICEVENTHANDLER_H
#define DD4HEP_DDEVE_GENERICEVENTHANDLER_H

// Framework include files
#include "DDEve/EventHandler.h"

// Forward declarations

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Event handler base class. Interface to all DDEve I/O actions
  /** 
   *  \author  M.Frank
   *  \version 1.0
   *  \ingroup DD4HEP_EVE
   */
  class GenericEventHandler : public EventHandler  {
  protected:
    /// Subscriber set
    typedef std::set<EventConsumer*> Subscriptions;
    std::map<std::string,EventHandler*> m_handlers;
    EventHandler* m_current;
    /// Data subscriptions (unordered)
    Subscriptions m_subscriptions;

    EventHandler* current() const;
  public:
    /// Standard constructor
    GenericEventHandler();
    /// Default destructor
    virtual ~GenericEventHandler();
    /// Access the map of simulation data collections
    virtual const TypedEventCollections& data()  const  override   { return current()->data();  }
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const  override;
    /// Access the data source name
    virtual std::string datasourceName() const  override;
    /// Access to the collection type by name
    virtual CollectionType collectionType(const std::string& collection) const  override;
    /// Loop over collection and extract data
    virtual size_t collectionLoop(const std::string& collection, DDEveHitActor& actor)  override;
    /// Loop over collection and extract particle data
    virtual size_t collectionLoop(const std::string& collection, DDEveParticleActor& actor)  override;
    /// Open a new event data file
    virtual bool Open(const std::string& type, const std::string& file_name)  override;
    /// Load the next event
    virtual bool NextEvent()  override;
    /// User overloadable function: Load the previous event
    virtual bool PreviousEvent()  override;
    /// Goto a specified event in the file
    virtual bool GotoEvent(long event_number)  override;
    /// Subscribe to notification of new data present
    virtual void Subscribe(EventConsumer* display);
    /// Unsubscribe from notification of new data present
    virtual void Unsubscribe(EventConsumer* display);

#ifndef __CINT__
    /// Notfy all subscribers
    virtual void NotifySubscribers(void (EventConsumer::*pmf)(EventHandler&));
#endif
 
    ClassDefOverride(GenericEventHandler,0);
  };
} /* End namespace dd4hep   */

#endif /* DD4HEP_DDEVE_GENERICEVENTHANDLER_H */

