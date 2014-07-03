// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
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

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /** @class DDG4EventHandler  DDG4EventHandler.h DDEve/DDG4EventHandler.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class DDG4EventHandler : public EventHandler  {
  public:
    typedef std::map<std::string,std::pair<TBranch*,void*> > Branches;
    typedef void* (*HitAccessor_t)(void*, DDEveHit*);
  protected:
    /// Reference to data file
    std::pair<TFile*,TTree*> m_file;
    /// Branch map
    Branches m_branches;
    /// File entry number
    Long64_t m_entry;
    /// 
    HitAccessor_t m_simhitConverter;
  public:
    /// Standard constructor
    DDG4EventHandler();
    /// Default destructor
    virtual ~DDG4EventHandler();

    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const;
    /// Access the data source name
    std::string datasourceName() const;
    /// Call functor on hit collection
    virtual size_t collectionLoop(const std::string& collection, DDEveHitActor& actor);
    /// Open new data file
    virtual bool Open(const std::string& file_name);
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

