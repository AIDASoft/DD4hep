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
// @author  P.Kostka (main author)
// @author  M.Frank  (code reshuffeling into new DDG4 scheme)
//
//====================================================================

// Framework include files
#include "DD4hep/Memory.h"
#include "DD4hep/Plugins.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4InputAction.h"

#include "G4Event.hh"

using namespace std;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

/// Initializing constructor
Geant4EventReader::Geant4EventReader(const std::string& nam)
  : m_name(nam), m_directAccess(false), m_currEvent(0)
{
}

/// Default destructor
Geant4EventReader::~Geant4EventReader()   {
}

/// Skip event. To be implemented for sequential sources
Geant4EventReader::EventReaderStatus Geant4EventReader::skipEvent()  {
  if ( hasDirectAccess() )   {
    ++m_currEvent;
    return EVENT_READER_OK;
  }
  std::vector<Particle*> particles;
  Geant4Vertex vertex;
  ++m_currEvent;
  EventReaderStatus sc = readParticles(m_currEvent,vertex,particles);
  for_each(particles.begin(),particles.end(),deleteObject<Particle>);
  return sc;
}

/// Move to the indicated event number.
Geant4EventReader::EventReaderStatus
Geant4EventReader::moveToEvent(int event_number)   {
  if ( event_number >= INT_MIN )   {
    return EVENT_READER_OK;  // Logic below does not work as expected.
  }                          // This shortcuts it!
                             // APS: would have been nice to know what exactly doesn't work...
  if ( m_currEvent == event_number )  {
    return EVENT_READER_OK;
  }
  else if ( hasDirectAccess() )   {
    m_currEvent = event_number;
    return EVENT_READER_OK;
  }
  else if ( event_number<m_currEvent )   {
    return EVENT_READER_ERROR;
  }
  else  {
    for(int i=m_currEvent; i<event_number;++i)
      skipEvent();
    m_currEvent = event_number;
    return EVENT_READER_OK;
  }
  return EVENT_READER_ERROR;
}

/// Standard constructor
Geant4InputAction::Geant4InputAction(Geant4Context* ctxt, const string& nam)
  : Geant4GeneratorAction(ctxt,nam), m_reader(0), m_currentEventNumber(0)
{
  declareProperty("Input",          m_input);
  declareProperty("Sync",           m_firstEvent=0);
  declareProperty("Mask",           m_mask = 0);
  declareProperty("MomentumScale",  m_momScale = 1.0);
  declareProperty("HaveAbort",      m_abort = true);
  m_needsControl = true;
}

/// Default destructor
Geant4InputAction::~Geant4InputAction()   {
}

/// helper to report Geant4 exceptions
string Geant4InputAction::issue(int i)  const  {
  stringstream str;
  str << "Geant4InputAction[" << name() << "]: Event " << i << " ";
  return str.str();
}

/// Read an event and return a LCCollection of MCParticles.
int Geant4InputAction::readParticles(int evt_number,
                                     Vertex& prim_vertex,
                                     std::vector<Particle*>& particles)
{
  int evid = evt_number + m_firstEvent;
  if ( 0 == m_reader )  {
    if ( m_input.empty() )  {
      throw runtime_error("InputAction: No input file declared!");
    }
    string err;
    TypeName tn = TypeName::split(m_input,"|");
    try  {
      m_reader = PluginService::Create<Geant4EventReader*>(tn.first,tn.second);
      if ( 0 == m_reader )   {
        PluginDebug dbg;
        m_reader = PluginService::Create<Geant4EventReader*>(tn.first,tn.second);
        abortRun(issue(evid)+"Error creating reader plugin.",
                 "Failed to create file reader of type %s. Cannot open dataset %s",
                 tn.first.c_str(),tn.second.c_str());
        return Geant4EventReader::EVENT_READER_NO_FACTORY;
      }
    }
    catch(const exception& e)  {
      err = e.what();
    }
    if ( !err.empty() )  {
      abortRun(issue(evid)+err,"Error when creating reader for file %s",m_input.c_str());
      return Geant4EventReader::EVENT_READER_NO_FACTORY;
    }
  }
  int status = m_reader->moveToEvent(evid);
  if ( Geant4EventReader::EVENT_READER_OK != status )  {
    string msg = issue(evid)+"Error when moving to event - may be end of file.";
    if ( m_abort )  {
      abortRun(msg,"Error when reading file %s",m_input.c_str());
      return status;
    }
    except("%s Error when reading file %s.", msg.c_str(), m_input.c_str());
    return status;
  }
  status = m_reader->readParticles(evid, prim_vertex, particles);
  if ( Geant4EventReader::EVENT_READER_OK != status )  {
    string msg = issue(evid)+"Error when moving to event - may be end of file.";
    if ( m_abort )  {
      abortRun(msg,"Error when reading file %s",m_input.c_str());
      return status;
    }
    except("%s Error when reading file %s.", msg.c_str(), m_input.c_str());
  }
  return status;
}

/// Callback to generate primary particles
void Geant4InputAction::operator()(G4Event* event)   {
  vector<Particle*>         primaries;
  Geant4Event&              evt = context()->event();
  Geant4PrimaryEvent*       prim = evt.extension<Geant4PrimaryEvent>();
  dd4hep_ptr<Geant4Vertex>  vertex(new Geant4Vertex());
  int result;

  vertex->x = 0;
  vertex->y = 0;
  vertex->z = 0;
  vertex->time = 0;
  result = readParticles(m_currentEventNumber, *(vertex.get()), primaries);

  event->SetEventID(m_firstEvent + m_currentEventNumber);
  ++m_currentEventNumber;

  if ( result != Geant4EventReader::EVENT_READER_OK )   {    // handle I/O error, but how?
    return;
  }

  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  prim->add(m_mask, inter);
  // check if there is at least one particle
  if ( primaries.empty() ) return;

  print("+++ Particle interaction with %d generator particles ++++++++++++++++++++++++",
        int(primaries.size()));
  Geant4Vertex* vtx = vertex.get();
  inter->vertices.insert(make_pair(m_mask,vertex.release())); // Move vertex ownership
  // build collection of MCParticles
  for(size_t i=0; i<primaries.size(); ++i )   {
    Geant4ParticleHandle p(primaries[i]);
    const double mom_scale = m_momScale;
    PropertyMask status(p->status);
    p->psx  = mom_scale*p->psx;
    p->psy  = mom_scale*p->psy;
    p->psz  = mom_scale*p->psz;

    if ( p->parents.size() == 0 )  {
      if ( status.isSet(G4PARTICLE_GEN_EMPTY) || status.isSet(G4PARTICLE_GEN_DOCUMENTATION) )
        vtx->in.insert(p->id);  // Beam particles and primary quarks etc.
      else
        vtx->out.insert(p->id); // Stuff, to be given to Geant4 together with daughters
    }
    inter->particles.insert(make_pair(p->id,p));
    p.dumpWithMomentumAndVertex(outputLevel()-1,name(),"->");
  }
}
