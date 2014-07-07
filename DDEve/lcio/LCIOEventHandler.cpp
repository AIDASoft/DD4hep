// $Id: LCDD.h 1117 2014-04-25 08:07:22Z markus.frank@cern.ch $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#include "LCIOEventHandler.h"
#include "DD4hep/Printout.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Factories.h"

#include "IO/LCReader.h"
#include "EVENT/LCCollection.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"
#include "EVENT/MCParticle.h"

#include "TSystem.h"
#include "TGMsgBox.h"

// C/C++ include files
#include <stdexcept>
#include <climits>

using namespace std;
using namespace lcio;
using namespace DD4hep;
using namespace EVENT;
using namespace IMPL;

const void* _fill(const SimTrackerHit* s, DDEveHit* target)   {
  const double* p = s->getPosition();
  target->x = p[0];
  target->y = p[1];
  target->z = p[2];
  target->deposit = s->getEDep();
  return s;
}
const void* _fill(const SimCalorimeterHit* s, DDEveHit* target)   {
  const float* p = s->getPosition();
  target->x = p[0];
  target->y = p[1];
  target->z = p[2];
  target->deposit = s->getEnergy();
  return s;
}

static const void* _convertHitFunc(const LCObject* source, DDEveHit* target)  {
  const SimTrackerHit* t = dynamic_cast<const SimTrackerHit*>(source);
  if (t && _fill(t,target)) return t;
  const SimCalorimeterHit* c = dynamic_cast<const SimCalorimeterHit*>(source);
  if (c && _fill(c,target)) return c;
  return 0;
}

static void* _create(const char*)  {
  EventHandler* h = new LCIOEventHandler();
  return h;
}
using namespace DD4hep::Geometry;
DECLARE_CONSTRUCTOR(DDEve_LCIOEventHandler,_create)

/// Standard constructor
LCIOEventHandler::LCIOEventHandler() : EventHandler(), m_lcReader(0), m_event(0) {
  m_lcReader = LCFactory::getInstance()->createLCReader() ;
}

/// Default destructor
LCIOEventHandler::~LCIOEventHandler()   {
  delete m_lcReader;
}

/// Access the number of events on the current input data source (-1 if no data source connected)
long LCIOEventHandler::numEvents() const   {
  if ( hasFile() )  {
    return m_lcReader->getNumberOfEvents();
  }
  return -1;
}

/// Call functor on hit collection
size_t LCIOEventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor)   {
  typedef std::vector<void*> _P;
  Branches::const_iterator i = m_branches.find(collection);
  if ( i != m_branches.end() )   {
    LCCollection* c = (*i).second;
    if ( c )  {
      DDEveHit hit;
      int n = c->getNumberOfElements();
      actor.setSize(n);
      for(int i=0; i<n; ++i)  {
	LCObject* ptr = c->getElementAt(i);
	if ( _convertHitFunc(ptr,&hit) )    {
	  actor(hit);
	}
      }
      return n;
    }
  }
  return 0;
}

/// Open new data file
bool LCIOEventHandler::Open(const std::string&, const std::string& name)   {
  string err;
  if ( m_hasFile ) m_lcReader->close();
  m_hasFile = false;
  m_hasEvent = false;
  m_event = 0;
  m_branches.clear();
  m_lcReader->open(name);
  m_hasFile = true;
  return true;
}

/// Load the next event
bool LCIOEventHandler::NextEvent()   {
  m_data.clear();
  m_hasEvent = false;
  m_branches.clear();
  m_data.clear();
  if ( hasFile() )  {
    m_event = m_lcReader->readNextEvent();
    if ( m_event )   {
      const std::vector<std::string>* collnames = m_event->getCollectionNames();
      for( std::vector< std::string >::const_iterator i = collnames->begin(); i != collnames->end(); i++){
	LCCollection* c = m_event->getCollection(*i);
	m_data[c->getTypeName()].push_back(make_pair((*i).c_str(),c->getNumberOfElements()));
	m_branches[*i] = c;
      }
      m_hasEvent = true;
      return 1;
    }
    throw runtime_error("+++ EventHandler::readEvent: Failed to read event");
  }
  throw runtime_error("+++ EventHandler::readEvent: No file open!");
}

/// Load the previous event
bool LCIOEventHandler::PreviousEvent()   {
  throw runtime_error("+++ This version of the LCIO reader can only access files sequentially!\n"
		      "+++ Access to the previous event is not supported.");
}

/// Goto a specified event in the file
bool LCIOEventHandler::GotoEvent(long /* event_number */)   {
  throw runtime_error("+++ This version of the LCIO reader can only access files sequentially!\n"
		      "+++ Random access is not supported.");
}
