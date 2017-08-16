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
#ifndef DD4HEP_DDEVE_LCIOEVENTHANDLER_H
#define DD4HEP_DDEVE_LCIOEVENTHANDLER_H

// Framework include files
#include "DDEve/EventHandler.h"
#include "lcio.h"

// C/C++ include files
#include <map>
#include <string>

// Forward declarations

/*
 *   dd4hep namespace declaration
 */
namespace dd4hep {

  /** @class LCIOEventHandler  LCIOEventHandler.h DDEve/LCIOEventHandler.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class LCIOEventHandler : public EventHandler  {
  public:
    typedef std::map<std::string,lcio::LCCollection*> Branches;
    typedef const void* (*HitAccessor_t)(const lcio::LCObject*, DDEveHit*);
  protected:
    /// Reference to data file reader
    lcio::LCReader* m_lcReader;
    lcio::LCEvent*  m_event;
    std::string     m_fileName;
    /// Branch map
    Branches        m_branches;
    /// Data collection map
    TypedEventCollections m_data;
  public:
    /// Standard constructor
    LCIOEventHandler();
    /// Default destructor
    virtual ~LCIOEventHandler();

    /// Access the map of simulation data collections
    virtual const TypedEventCollections& data()  const   { return m_data;  }
    /// Access the data source name
    std::string datasourceName() const { return hasFile() ? m_fileName : std::string("UNKNOWN"); }
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const;
    /// Access to the collection type by name
    virtual CollectionType collectionType(const std::string& collection) const;
    /// Call functor on hit collection
    virtual size_t collectionLoop(const std::string& collection, DDEveHitActor& actor);
    /// Loop over collection and extract particle data
    virtual size_t collectionLoop(const std::string& collection, DDEveParticleActor& actor);
    /// Open new data file
    virtual bool Open(const std::string& type, const std::string& file_name);
    /// User overloadable function: Load the next event
    virtual bool NextEvent();
    /// User overloadable function: Load the previous event
    virtual bool PreviousEvent();
    /// Goto a specified event in the file
    virtual bool GotoEvent(long event_number);
  };

} /* End namespace dd4hep   */

#endif /* DD4HEP_DDEVE_LCIOEVENTHANDLER_H */

