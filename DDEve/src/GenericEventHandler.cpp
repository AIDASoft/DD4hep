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

// Framework include files
#include <DDEve/GenericEventHandler.h>
#include <DD4hep/Primitives.h>
#include <DD4hep/Factories.h>
#include <DD4hep/Plugins.h>
#include <stdexcept>

/// ROOT include files
#include <TROOT.h>
#include <TGMsgBox.h>
#include <TSystem.h>
#include <climits>

using namespace dd4hep;

ClassImp(GenericEventHandler)

/// Standard constructor
GenericEventHandler::GenericEventHandler() : m_current(0) {
}

/// Default destructor
GenericEventHandler::~GenericEventHandler()  {
  m_subscriptions.clear();
  detail::deletePtr(m_current);
}

EventHandler* GenericEventHandler::current() const   {
  if ( m_current )  {
    return m_current;
  }
  throw std::runtime_error("Invalid event handler");
}

/// Notfy all subscribers
void GenericEventHandler::NotifySubscribers(void (EventConsumer::*pmf)(EventHandler&))   {
  for(Subscriptions::iterator i=m_subscriptions.begin(); i!=m_subscriptions.end();++i)
    ((*i)->*pmf)(*this);
}

/// Subscribe to notification of new data present
void GenericEventHandler::Subscribe(EventConsumer* consumer)  {
  m_subscriptions.insert(consumer);
}

/// Unsubscribe from notification of new data present
void GenericEventHandler::Unsubscribe(EventConsumer* consumer)  {
  Subscriptions::iterator i=m_subscriptions.find(consumer);
  if ( i != m_subscriptions.end() ) m_subscriptions.erase(i);
}

/// Access the number of events on the current input data source (-1 if no data source connected)
long GenericEventHandler::numEvents() const   {
  return current()->numEvents();
}

/// Access the data source name
std::string GenericEventHandler::datasourceName() const {
  return current()->datasourceName();
}

/// Access to the collection type by name
EventHandler::CollectionType GenericEventHandler::collectionType(const std::string& collection) const   {
  if ( m_current && m_current->hasEvent() )  {
    return m_current->collectionType(collection);
  }
  return NO_COLLECTION;
}

/// Loop over collection and extract data
std::size_t GenericEventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor) {
  if ( m_current && m_current->hasEvent() )  {
    return m_current->collectionLoop(collection,actor);
  }
  return 0;
}

/// Loop over collection and extract particle data
std::size_t GenericEventHandler::collectionLoop(const std::string& collection, DDEveParticleActor& actor)    {
  if ( m_current && m_current->hasEvent() )  {
    return m_current->collectionLoop(collection,actor);
  }
  return 0;
}

/// Open a new event data file
bool GenericEventHandler::Open(const std::string& file_type, const std::string& file_name)   {
  std::size_t idx = file_name.find("lcio");
  std::size_t idr = file_name.find("root");
  std::string err;
  m_hasFile = false;
  m_hasEvent = false;
  try  {
    detail::deletePtr(m_current);
    //  prefer event handler configured in xml
    if ( file_type.find("FCC") != std::string::npos ) {
      m_current = (EventHandler*)PluginService::Create<void*>("DD4hep_DDEve_FCCEventHandler",(const char*)0);
    }
    // fall back to defaults according to file ending
    else if ( idx != std::string::npos )   {
      m_current = (EventHandler*)PluginService::Create<void*>("DD4hep_DDEve_LCIOEventHandler",(const char*)0);
    }
    else if ( idr != std::string::npos )   {
      m_current = (EventHandler*)PluginService::Create<void*>("DD4hep_DDEve_DDG4EventHandler",(const char*)0);
    }
    else   {
      throw std::runtime_error("Attempt to open file:"+file_name+" of unknown type:"+file_type);
    }
    if ( m_current )   {
      if ( m_current->Open(file_type, file_name) )   {
        m_hasFile = true;
        NotifySubscribers(&EventConsumer::OnFileOpen);
        return true;
      }
      err = "+++ Failed to open the data file:"+file_name;
      detail::deletePtr(m_current);   
    }
    else  {
      err = "+++ Failed to create fikle reader for file '"+file_name+"' of type '"+file_type+"'";
    }
  }
  catch(const std::exception& e)  {
    err = "\nAn exception occurred \n"
      "while opening event data:\n" + std::string(e.what()) + "\n\n";
  }
  std::string path = TString::Format("%s/stop_t.xpm", TROOT::GetIconPath().Data()).Data();
  const TGPicture* pic = gClient->GetPicture(path.c_str());
  new TGMsgBox(gClient->GetRoot(),0,"Failed to open event data",err.c_str(),pic,
               kMBDismiss,0,kVerticalFrame,kTextLeft|kTextCenterY);
  return false;
}

/// Load the next event
bool GenericEventHandler::NextEvent()   {
  m_hasEvent = false;
  try {
    if ( m_hasFile )   {
      if ( current()->NextEvent() > 0 )   {
        m_hasEvent = true;
        NotifySubscribers(&EventConsumer::OnNewEvent);
        return 1;
      }
    }
    throw std::runtime_error("+++ EventHandler::readEvent: No file open!");
  }
  catch(const std::exception& e)  {
    std::string path = TString::Format("%s/stop_t.xpm", TROOT::GetIconPath().Data()).Data();
    std::string err = "\nAn exception occurred \n"
      "while reading a new event:\n" + std::string(e.what()) + "\n\n";
    const TGPicture* pic = gClient->GetPicture(path.c_str());
    new TGMsgBox(gClient->GetRoot(),0,"Failed to read event", err.c_str(),pic,
                 kMBDismiss,0,kVerticalFrame,kTextLeft|kTextCenterY);
  }
  return -1;
}

/// User overloadable function: Load the previous event
bool GenericEventHandler::PreviousEvent()    {
  m_hasEvent = false;
  if ( m_hasFile && current()->PreviousEvent() > 0 )   {
    m_hasEvent = true;
    NotifySubscribers(&EventConsumer::OnNewEvent);
    return 1;
  }
  return -1;
}

/// Goto a specified event in the file
bool GenericEventHandler::GotoEvent(long event_number)   {
  m_hasEvent = false;
  if ( m_hasFile && current()->GotoEvent(event_number) > 0 )   {
    m_hasEvent = true;
    NotifySubscribers(&EventConsumer::OnNewEvent);
    return 1;
  }
  return -1;
}
