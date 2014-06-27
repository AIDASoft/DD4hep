// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDEVE_EVENTHANDLER_H
#define DD4HEP_DDEVE_EVENTHANDLER_H

// Framework include files
#include "TClass.h"
class TH2F;

// C/C++ include files
#include <set>
#include <map>
#include <vector>

// Forward declarations

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  class EventConsumer;

  /** @class EventHandler  EventHandler.h DDEve/EventHandler.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class EventHandler   {
  public:
    typedef std::pair<const char*,std::vector<void*>* > Collection;
    typedef std::map<std::string,std::vector<Collection> > Data;
    typedef std::set<EventConsumer*> Subscriptions;

    typedef std::pair<const std::type_info*,std::string> DataID;
    typedef void (*fcn)(void*);
    typedef std::pair<fcn,void*> DataHolder;
    typedef std::map<DataID, DataHolder> EventCollections;
  protected:
    /// Data subscriptions (unordered)
    Subscriptions m_subscriptions;
    EventCollections m_collections;
    bool m_hasEvent;
  public:
    /// Standard constructor
    EventHandler();
    /// Default destructor
    virtual ~EventHandler();

    template <typename T> std::vector<const T*> GetHits(const std::string& collection);
    /// Access the map of simulation data collections
    virtual const Data& data()  const = 0;
    /// Check if a data file is connected to the handler
    virtual bool hasFile() const = 0;
    virtual bool hasEvent() const  { return m_hasEvent; }
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const = 0;
    /// Access the data source name
    virtual std::string datasourceName() const = 0;

    /// Clear all event related data caches
    virtual void ClearCache();

    /// Open a new event data file
    virtual bool Open(const std::string& file_name) = 0;

    /// Load the next event
    virtual bool NextEvent() = 0;
    /// User overloadable function: Load the previous event
    virtual bool PreviousEvent() = 0;
    /// Goto a specified event in the file
    virtual bool GotoEvent(long event_number) = 0;

    /// Subscribe to notification of new data present
    virtual void Subscribe(EventConsumer* display);
    /// Unsubscribe from notification of new data present
    virtual void Unsubscribe(EventConsumer* display);

    /// Fill eta-phi histogram from a hit collection
    virtual size_t FillEtaPhiHistogram(const std::string& collection, TH2F* histogram)  = 0;

#ifndef __CINT__
    /// Notfy all subscribers
    virtual void NotifySubscribers(void (EventConsumer::*pmf)(EventHandler*));
#endif
    ClassDef(EventHandler,0);
  };

  /** @class EventConsumer  EventHandler.h DDEve/EventHandler.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class EventConsumer   {
  public:
    /// Standard constructor
    EventConsumer();
    /// Default destructor
    virtual ~EventConsumer();
    /// Consumer event data callback
    virtual void OnNewEvent(EventHandler* /* handler */) {}
    /// Consumer file open callback
    virtual void OnFileOpen(EventHandler* /* handler */) {}

    ClassDef(EventConsumer,0);
  };

} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_EVENTHANDLER_H */

