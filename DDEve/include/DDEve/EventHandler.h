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
#ifndef DD4HEP_DDEVE_EVENTHANDLER_H
#define DD4HEP_DDEVE_EVENTHANDLER_H

// Framework include files
#include "DDEve/DDEveEventData.h"
#include "TClass.h"
class TH2F;

// C/C++ include files
#include <set>
#include <map>
#include <vector>

// Forward declarations
#define MEV_2_GEV 0.001

/*
 *   dd4hep namespace declaration
 */
namespace dd4hep {

  // Forward declarations
  class EventConsumer;
  
  /// Event data actor base class for hits. Used to extract data from concrete classes.
  /** 
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  struct DDEveHitActor  {
    virtual ~DDEveHitActor() {}
    virtual void operator()(const DDEveHit&) = 0;
    virtual void setSize(size_t /* num_elements */) {}
  };

  /// Event data actor base class for particles. Used to extract data from concrete classes.
  /** 
   * @author  M.Frank
   * @version 1.0
   */
  struct DDEveParticleActor  {
    virtual ~DDEveParticleActor() {}
    virtual void operator()(const DDEveParticle&) = 0;
    virtual void setSize(size_t /* num_elements */) {}
  };

  /// Event handler base class. Interface to all DDEve I/O actions
  /** 
   * @author  M.Frank
   * @version 1.0
   */
  class EventHandler   {
  public:
    enum CollectionType { 
      NO_COLLECTION=1<<0, 
      PARTICLE_COLLECTION=1<<1, 
      TRACK_COLLECTION=1<<2,
      CALO_HIT_COLLECTION=1<<3,
      TRACKER_HIT_COLLECTION=1<<4,
      HIT_COLLECTION=CALO_HIT_COLLECTION|TRACKER_HIT_COLLECTION
    };
    /// Collection definition: name, size
    typedef std::pair<const char*,size_t> Collection;
    /// Types collection: collections are grouped by type (class name)
    typedef std::map<std::string,std::vector<Collection> > TypedEventCollections;

  protected:
    /// Flag to indicate that a file is opened
    bool m_hasFile = false;
    /// Flag to indicate that an event is loaded
    bool m_hasEvent = false;
  public:
    /// Standard constructor
    EventHandler() = default;
    /// Default destructor
    virtual ~EventHandler();
    /// Check if an event is present in memory
    virtual bool hasEvent() const  { return m_hasEvent; }
    /// Check if a data file is connected to the handler
    virtual bool hasFile() const   { return m_hasFile;  }
    /// Access the map of simulation data collections
    virtual const TypedEventCollections& data()  const = 0;
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const = 0;
    /// Access the data source name
    virtual std::string datasourceName() const = 0;
    /// Loop over collection and extract hit data
    virtual size_t collectionLoop(const std::string& collection, DDEveHitActor& actor) = 0;
    /// Loop over collection and extract particle data
    virtual size_t collectionLoop(const std::string& collection, DDEveParticleActor& actor) = 0;
    /// Access to the collection type by name
    virtual CollectionType collectionType(const std::string& collection) const = 0;
    /// Open a new event data file
    virtual bool Open(const std::string& type, const std::string& file_name) = 0;
    /// Load the next event
    virtual bool NextEvent() = 0;
    /// User overloadable function: Load the previous event
    virtual bool PreviousEvent() = 0;
    /// Goto a specified event in the file
    virtual bool GotoEvent(long event_number) = 0;

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
    EventConsumer() = default;
    /// Default destructor
    virtual ~EventConsumer();
    /// Consumer event data callback
    virtual void OnNewEvent(EventHandler& /* handler */) = 0;
    /// Consumer file open callback
    virtual void OnFileOpen(EventHandler& /* handler */) = 0;

    ClassDef(EventConsumer,0);
  };
}      /* End namespace dd4hep        */
#endif /* DD4HEP_DDEVE_EVENTHANDLER_H */

