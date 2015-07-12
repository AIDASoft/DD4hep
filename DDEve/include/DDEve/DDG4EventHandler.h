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
#ifndef DD4HEP_DDEVE_DDG4EVENTHANDLER_H
#define DD4HEP_DDEVE_DDG4EVENTHANDLER_H

// Framework include files
#include "DDEve/EventHandler.h"

// C/C++ include files
#include <map>
#include <string>

// Forward declarations
class TTree;
class TFile;
class TBranch;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /** Event I/O handler class for the DD4hep event display
   *
   * \author  M.Frank
   * \version 1.0
   * \ingroup DD4HEP_EVE
   */
  class DDG4EventHandler : public EventHandler  {
  public:
    typedef std::map<std::string,std::pair<TBranch*,void*> > Branches;
    typedef void* (*ParticleAccessor_t)(void*, DDEveParticle*);
    typedef void* (*HitAccessor_t)(void*, DDEveHit*);
  protected:
    /// Reference to data file
    std::pair<TFile*,TTree*> m_file;
    /// Branch map
    Branches m_branches;
    /// File entry number
    Long64_t m_entry;
    /// Function pointer to interprete hits
    HitAccessor_t m_simhitConverter;
    /// Function pointer to interprete particles
    ParticleAccessor_t m_particleConverter;
    /// Data collection map
    TypedEventCollections m_data;
  public:
    /// Standard constructor
    DDG4EventHandler();
    /// Default destructor
    virtual ~DDG4EventHandler();

    /// Access the map of simulation data collections
    virtual const TypedEventCollections& data()  const   { return m_data;  }
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const;
    /// Access the data source name
    std::string datasourceName() const;
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
    /// Load the specified event
    Int_t ReadEvent(Long64_t n);

    ClassDef(DDG4EventHandler,0);
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_DDG4EVENTHANDLER_H */

