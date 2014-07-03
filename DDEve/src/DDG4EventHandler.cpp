// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "DDEve/DDG4EventHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Factories.h"

#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TSystem.h"
#include "TBranch.h"
#include "TGMsgBox.h"

// C/C++ include files
#include <stdexcept>
#include <climits>

using namespace std;
using namespace DD4hep;

ClassImp(DDG4EventHandler)
namespace {
  union FCN  {
    FCN(void* p) { ptr = p; }
    DDG4EventHandler::HitAccessor_t func;
    void* ptr;
  };
}

/// Standard constructor
DDG4EventHandler::DDG4EventHandler() : EventHandler(), m_file(0,0), m_entry(-1) {
  void* ptr = ROOT::Reflex::PluginService::Create<void*>("DDEve_DDG4HitAccess",(const char*)"");
  if ( 0 == ptr )   {
    throw runtime_error("FATAL: Failed to access function pointer from factory DDEve_DDG4HitAccess");
  }
  m_simhitConverter = FCN(ptr).func;
}

/// Default destructor
DDG4EventHandler::~DDG4EventHandler()   {
  if ( m_file.first )  {
    m_file.first->Close();
    delete m_file.first;
  }
  m_file.first = 0;
  m_file.second = 0;
}

/// Load the next event
bool DDG4EventHandler::NextEvent()   {
  return ReadEvent(++m_entry) > 0;
}

/// Load the previous event
bool DDG4EventHandler::PreviousEvent()   {
  return ReadEvent(--m_entry) > 0;
}

/// Goto a specified event in the file
bool DDG4EventHandler::GotoEvent(long event_number)   {
  return ReadEvent(m_entry = event_number) > 0;
}

/// Access the number of events on the current input data source (-1 if no data source connected)
long DDG4EventHandler::numEvents() const   {
  if ( hasFile() )  {
    return m_file.second->GetEntries();
  }
  return -1;
}

/// Access the data source name
std::string DDG4EventHandler::datasourceName() const   {
  if ( hasFile() )  {
    return m_file.first->GetName();
  }
  return "UNKNOWN";
}

/// Call functor on hit collection
size_t DDG4EventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor)   {
  typedef std::vector<void*> _P;
  Branches::const_iterator i = m_branches.find(collection);
  if ( i != m_branches.end() )   {
    const _P* data = (_P*)(*i).second.second;
    if ( data )  {
      DDEveHit hit;
      actor.setSize(data->size());
      for(_P::const_iterator i=data->begin(); i!=data->end(); ++i)   {
	if ( (*m_simhitConverter)(*i,&hit) )    {
	  actor(hit);
	}
      }
      return data->size();
    }
  }
  return 0;
}

/// Load the specified event
Int_t DDG4EventHandler::ReadEvent(Long64_t event_number)   {
  ClearCache();
  m_data.clear();
  m_hasEvent = false;
  try {
    if ( hasFile() )  {
      if ( event_number >= m_file.second->GetEntries() )  {
	event_number = m_file.second->GetEntries()-1;
	printout(ERROR,"DDG4EventHandler","+++ ReadEvent: Cannot read across End-of-file! Reading last event:%d.",event_number);
      }
      else if ( event_number < 0 )  {
	event_number = 0;
	printout(ERROR,"DDG4EventHandler","+++ nextEvent: Cannot read across Start-of-file! Reading first event:%d.",event_number);
      }

      Int_t nbytes = m_file.second->GetEntry(event_number);
      if ( nbytes >= 0 )   {
	printout(ERROR,"DDG4EventHandler","+++ ReadEvent: Read %d bytes of event data for entry:%d",nbytes,event_number);
	for(Branches::const_iterator i=m_branches.begin(); i != m_branches.end(); ++i)  {
	  TBranch* b = (*i).second.first;
	  std::vector<void*>* data = *(std::vector<void*>**)b->GetAddress();
	  m_data[b->GetClassName()].push_back(make_pair(b->GetName(),data->size()));
	}
	m_hasEvent = true;
	NotifySubscribers(&EventConsumer::OnNewEvent);
	return nbytes;
      }
      printout(ERROR,"DDG4EventHandler","+++ ReadEvent: Cannot read event data for entry:%d",event_number);
      throw runtime_error("+++ EventHandler::readEvent: Failed to read event");
    }
    throw runtime_error("+++ EventHandler::readEvent: No file open!");
  }
  catch(const std::exception& e)  {
    string path = TString::Format("%s/icons/stop_t.xpm", gSystem->Getenv("ROOTSYS")).Data();
    string err = "\nAn exception occurred \n"
      "while reading a new event:\n" + string(e.what()) + "\n\n";
    const TGPicture* pic = gClient->GetPicture(path.c_str());
    new TGMsgBox(gClient->GetRoot(),0,"Failed to read event", err.c_str(),pic);
  }
  return -1;
}

/// Open new data file
bool DDG4EventHandler::Open(const std::string& name)   {
  string err;
  if ( m_file.first ) m_file.first->Close();
  m_hasFile = false;
  m_hasEvent = false;
  try {
    char buff[PATH_MAX];
    string dir = ::getcwd(buff,sizeof(buff));
    TFile* f = TFile::Open(name.c_str());
    if ( f && !f->IsZombie() )  {
      m_file.first = f;
      TTree* t = (TTree*)f->Get("EVENT");
      if ( t )   {
	TObjArray* br = t->GetListOfBranches();
	m_file.second = t;
	m_entry = -1;
	m_branches.clear();
	for(Int_t i=0; i<br->GetSize(); ++i)  {
	  TBranch* b = (TBranch*)br->At(i);
	  if ( !b ) continue;
	  m_branches[b->GetName()] = make_pair(b,(void*)0);
	  printout(INFO,"DDG4EventHandler::open","+++ Branch %s has %ld entries.",b->GetName(),b->GetEntries());
	}
	for(Int_t i=0; i<br->GetSize(); ++i)  {
	  TBranch* b = (TBranch*)br->At(i);
	  if ( !b ) continue;
	  b->SetAddress(&m_branches[b->GetName()].second);
	}
	m_hasFile = true;
	NotifySubscribers(&EventConsumer::OnFileOpen);
	return true;
      }
      err = "+++ Failed to access tree EVENT in ROOT file:"+name+" in "+dir;
    }
    else {
      err = "+++ Failed to open ROOT file:"+name+" in "+dir;
    }
  }
  catch(const std::exception& e)  {
    err = "\nAn exception occurred \n"
      "while opening event data:\n" + string(e.what()) + "\n\n";
  }
  string path = TString::Format("%s/icons/stop_t.xpm", gSystem->Getenv("ROOTSYS")).Data();
  const TGPicture* pic = gClient->GetPicture(path.c_str());
  new TGMsgBox(gClient->GetRoot(),0,"Failed to open event data", err.c_str(),pic);
  return false;
}

