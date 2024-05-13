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
#include <DD4hep/Memory.h>
#include <DD4hep/Plugins.h>
#include <DDG4/Geant4Primary.h>
#include <DDG4/Geant4Context.h>
#include <DDG4/Geant4Kernel.h>
#include <DDG4/Geant4InputAction.h>
#include <DDG4/Geant4RunAction.h>

#include <G4Event.hh>

using namespace dd4hep::sim;
using Vertices = Geant4InputAction::Vertices;
using PropertyMask = dd4hep::detail::ReferenceBitMask<int>;


/// Initializing constructor
Geant4EventReader::Geant4EventReader(const std::string& nam) : m_name(nam)
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
  for_each(vertices.begin(),vertices.end(),detail::deleteObject<Vertex>);
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
Geant4InputAction::Geant4InputAction(Geant4Context* ctxt, const std::string& nam)
  : Geant4GeneratorAction(ctxt,nam), m_reader(0), m_currentEventNumber(0)
{
  declareProperty("Input",          m_input);
  declareProperty("Sync",           m_firstEvent=0);
  declareProperty("Mask",           m_mask = 0);
  declareProperty("MomentumScale",  m_momScale = 1.0);
  declareProperty("HaveAbort",      m_abort = true);
  declareProperty("Parameters",     m_parameters = {});
  declareProperty("AlternativeDecayStatuses", m_alternativeDecayStatuses = {});
  m_needsControl = true;

  runAction().callAtBegin(this, &Geant4InputAction::beginRun);
}

/// Default destructor
Geant4InputAction::~Geant4InputAction()   {
}

///Intialize the event reader before the run starts
void Geant4InputAction::beginRun(const G4Run*) {
  createReader();
}

void Geant4InputAction::createReader() {
  if(m_reader) {
    return;
  }
  if ( m_input.empty() )  {
    except("InputAction: No input file declared!");
  }
  std::string err;
  TypeName tn = TypeName::split(m_input,"|");
  try  {
    m_reader = PluginService::Create<Geant4EventReader*>(tn.first,tn.second);
    if ( 0 == m_reader )   {
      PluginDebug dbg;
      m_reader = PluginService::Create<Geant4EventReader*>(tn.first,tn.second);
      abortRun("Error creating reader plugin.",
               "Failed to create file reader of type %s. Cannot open dataset %s",
               tn.first.c_str(),tn.second.c_str());
    }
    m_reader->setParameters( m_parameters );
    m_reader->checkParameters( m_parameters );
    m_reader->setInputAction( this );
    m_reader->registerRunParameters();
  } catch(const std::exception& e)  {
    err = e.what();
  }
  if ( !err.empty() )  {
    abortRun(err,"Error when creating reader for file %s",m_input.c_str());
  }
}


/// helper to report Geant4 exceptions
std::string Geant4InputAction::issue(int i)  const  {
  std::stringstream str;
  str << "Geant4InputAction[" << name() << "]: Event " << i << " ";
  return str.str();
}

/// Read an event and return a LCCollection of MCParticles.
int Geant4InputAction::readParticles(int evt_number,
                                     Vertices& vertices,
                                     std::vector<Particle*>& particles)
{
  //in case readParticles is called directly outside of having a run, we make sure a reader exists
  createReader();
  int evid = evt_number + m_firstEvent;
  int status = m_reader->moveToEvent(evid);
  if(status == Geant4EventReader::EVENT_READER_EOF ) {
    long nEvents = context()->kernel().property("NumEvents").value<long>();
    if(nEvents < 0) {
      //context()->kernel().runManager().AbortRun(true);
      throw DD4hep_End_Of_File();
    }
  }

  if ( Geant4EventReader::EVENT_READER_OK != status )  {
    std::string msg = issue(evid)+"Error when moving to event - ";
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
  if(status == Geant4EventReader::EVENT_READER_EOF ) {
    long nEvents = context()->kernel().property("NumEvents").value<long>();
    if(nEvents < 0) {
      //context()->kernel().runManager().AbortRun(true);
      throw DD4hep_End_Of_File();
    }
  }

  if ( Geant4EventReader::EVENT_READER_OK != status )  {
    std::string msg = issue(evid)+"Error when moving to event - ";
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
  std::vector<Particle*>    primaries;
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
    inter->vertices[m_mask].emplace_back( vertices[i] ); 
  }

  // build collection of MCParticles
  for(auto* primPart : primaries)   {
    Geant4ParticleHandle p(primPart);
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

    inter->particles.emplace(p->id,p);
    p.dumpWithMomentumAndVertex(outputLevel()-1,name(),"->");
  }
}

void Geant4InputAction::setGeneratorStatus(int genStatus, PropertyMask& status) {
  if ( genStatus == 0 ) status.set(G4PARTICLE_GEN_EMPTY);
  else if ( genStatus == 1 ) status.set(G4PARTICLE_GEN_STABLE);
  else if ( genStatus == 2 ) status.set(G4PARTICLE_GEN_DECAYED);
  else if ( genStatus == 3 ) status.set(G4PARTICLE_GEN_DOCUMENTATION);
  else if ( genStatus == 4 ) status.set(G4PARTICLE_GEN_BEAM);
  else if ( m_alternativeDecayStatuses.count(genStatus) ) status.set(G4PARTICLE_GEN_DECAYED);
  else
    status.set(G4PARTICLE_GEN_OTHER);

  return;
}
