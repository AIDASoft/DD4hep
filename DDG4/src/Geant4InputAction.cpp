//==========================================================================
//  AIDA Detector description implementation 
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
using namespace dd4hep::sim;
typedef dd4hep::detail::ReferenceBitMask<int> PropertyMask;
typedef Geant4InputAction::Vertices Vertices ;


/// Initializing constructor
Geant4EventReader::Geant4EventReader(const std::string& nam)
  : m_name(nam), m_directAccess(false), m_currEvent(0), m_inputAction(0)
{
}

/// Default destructor
Geant4EventReader::~Geant4EventReader()   {
}

/// Get the context (from the input action)
Geant4Context* Geant4EventReader::context() const {
  if( 0 == m_inputAction ) {
    printout(FATAL,"Geant4EventReader", "No input action registered!");
    throw std::runtime_error("Geant4EventReader: No input action registered!");
  }
  return m_inputAction->context();
}

/// Set the input action
void Geant4EventReader::setInputAction(Geant4InputAction* action) {
  m_inputAction = action;
}

/// Skip event. To be implemented for sequential sources
Geant4EventReader::EventReaderStatus Geant4EventReader::skipEvent()  {
  if ( hasDirectAccess() )   {
    ++m_currEvent;
    return EVENT_READER_OK;
  }
  std::vector<Particle*> particles;
  Vertices vertices ;
  
  ++m_currEvent;
  EventReaderStatus sc = readParticles(m_currEvent,vertices,particles);
  for_each(particles.begin(),particles.end(),detail::deleteObject<Particle>);
  return sc;
}

/// check if all parameters have been consumed by the reader, otherwise throws exception
void Geant4EventReader::checkParameters(std::map< std::string, std::string > &parameters) {

  if( parameters.empty() ) {
    return;
  }
  for (auto const& pairNV : parameters ) {
    printout(FATAL,"EventReader::checkParameters","Unknown parameter name: %s with value %s",
	     pairNV.first.c_str(),
	     pairNV.second.c_str());
  }
  throw std::runtime_error("Unknown parameter for event reader");

}

#if 0
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
#else
/// Move to the indicated event number.
Geant4EventReader::EventReaderStatus
Geant4EventReader::moveToEvent(int /* event_number */)   {
  return EVENT_READER_OK;
}
#endif

/// Standard constructor
Geant4InputAction::Geant4InputAction(Geant4Context* ctxt, const string& nam)
  : Geant4GeneratorAction(ctxt,nam), m_reader(0), m_currentEventNumber(0)
{
  declareProperty("Input",          m_input);
  declareProperty("Sync",           m_firstEvent=0);
  declareProperty("Mask",           m_mask = 0);
  declareProperty("MomentumScale",  m_momScale = 1.0);
  declareProperty("HaveAbort",      m_abort = true);
  declareProperty("Parameters",     m_parameters = {});
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
                                     Vertices& vertices,
                                     std::vector<Particle*>& particles)
{
  int evid = evt_number + m_firstEvent;
  if ( 0 == m_reader )  {
    if ( m_input.empty() )  {
      except("InputAction: No input file declared!");
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
      m_reader->setParameters( m_parameters );
      m_reader->checkParameters( m_parameters );
      m_reader->setInputAction( this );
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
    string msg = issue(evid)+"Error when moving to event - ";
    if ( status == Geant4EventReader::EVENT_READER_EOF ) msg += " EOF: [end of file].";
    else msg += " Unknown error condition";
    if ( m_abort )  {
      abortRun(msg,"Error when reading file %s",m_input.c_str());
      return status;
    }
    error(msg.c_str());
    except("Error when reading file %s.", m_input.c_str());
    return status;
  }
  status = m_reader->readParticles(evid, vertices, particles);


  if ( Geant4EventReader::EVENT_READER_OK != status )  {
    string msg = issue(evid)+"Error when moving to event - ";
    if ( status == Geant4EventReader::EVENT_READER_EOF ) msg += " EOF: [end of file].";
    else msg += " Unknown error condition";
    if ( m_abort )  {
      abortRun(msg,"Error when reading file %s",m_input.c_str());
      return status;
    }
    error(msg.c_str());
    except("Error when reading file %s.", m_input.c_str());
  }
  return status;
}

/// Callback to generate primary particles
void Geant4InputAction::operator()(G4Event* event)   {
  vector<Particle*>         primaries;
  Geant4Event&              evt = context()->event();
  Geant4PrimaryEvent*       prim = evt.extension<Geant4PrimaryEvent>();
  Vertices                  vertices ;
  int result;

  result = readParticles(m_currentEventNumber, vertices, primaries);

  event->SetEventID(m_firstEvent + m_currentEventNumber);
  ++m_currentEventNumber;

  if ( result != Geant4EventReader::EVENT_READER_OK )   {    // handle I/O error, but how?
    return;
  }

  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();
  prim->add(m_mask, inter);

  // check if there is at least one particle
  if ( primaries.empty() ) return;

  // check if there is at least one primary vertex
  if ( vertices.empty() ) return;

  print("+++ Particle interaction with %d generator particles and %d vertices ++++++++++++++++++++++++",
        int(primaries.size()), int(vertices.size()) );
  

  for(size_t i=0; i<vertices.size(); ++i )   {
    inter->vertices[m_mask].push_back( vertices[i] ); 
  }

  // build collection of MCParticles
  for(size_t i=0; i<primaries.size(); ++i )   {
    Geant4ParticleHandle p(primaries[i]);
    const double mom_scale = m_momScale;
    PropertyMask status(p->status);
    p->psx  = mom_scale*p->psx;
    p->psy  = mom_scale*p->psy;
    p->psz  = mom_scale*p->psz;

    //FIXME: this needs to be done now in the readers ...
    // // if ( p->parents.size() == 0 )  {
    // //   if ( status.isSet(G4PARTICLE_GEN_EMPTY) || status.isSet(G4PARTICLE_GEN_DOCUMENTATION) )
    // //     vtx->in.insert(p->id);  // Beam particles and primary quarks etc.
    // //   else
    // //     vtx->out.insert(p->id); // Stuff, to be given to Geant4 together with daughters
    // // }


    inter->particles.insert(make_pair(p->id,p));
    p.dumpWithMomentumAndVertex(outputLevel()-1,name(),"->");
  }



}
