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

  // Forward declarations
  namespace Simulation { class SimpleHit; }

  /** @class DDG4EventHandler  DDG4EventHandler.h DDEve/DDG4EventHandler.h
   *
   * @author  M.Frank
   * @version 1.0
   */
  class DDG4EventHandler : public EventHandler  {
  public:
    typedef Simulation::SimpleHit Hit;
    typedef std::map<std::string,std::pair<TBranch*,void*> > Branches;
  protected:
    /// Reference to data file
    std::pair<TFile*,TTree*> m_file;
    /// Branch map
    Branches m_branches;
    /// Data collection map
    Data     m_data;
    /// File entry number
    Long64_t m_entry;

  public:
    /// Standard constructor
    DDG4EventHandler();
    /// Default destructor
    virtual ~DDG4EventHandler();

    const Branches& branches()  const   {  return m_branches; }
    /// Access the map of simulation data collections
    virtual const Data& data()  const { return m_data;  }
    /// Check if a data file is connected to the handler
    virtual bool hasFile() const;
    /// Access the number of events on the current input data source (-1 if no data source connected)
    virtual long numEvents() const;
    /// Access the data source name
    std::string datasourceName() const;

    /// Open new data file
    virtual bool Open(const std::string& file_name);

    /// User overloadable function: Load the next event
    virtual bool NextEvent();
    /// User overloadable function: Load the previous event
    virtual bool PreviousEvent();
    /// Goto a specified event in the file
    virtual bool GotoEvent(long event_number);

    /// Fill eta-phi histogram from a hit collection
    virtual size_t FillEtaPhiHistogram(const std::string& collection, TH2F* histogram);

    /// Load the specified event
    Int_t ReadEvent(Long64_t n);

    ClassDef(DDG4EventHandler,0);
  };


} /* End namespace DD4hep   */


#endif /* DD4HEP_DDEVE_DDG4EVENTHANDLER_H */

