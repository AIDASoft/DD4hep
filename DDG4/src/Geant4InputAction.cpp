// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
// @author  P.Kostka (main author)
// @author  M.Frank  (code reshuffeling into new DDG4 scheme)
//
//====================================================================

// Framework include files
#include "DD4hep/Printout.h"
#include "DD4hep/Primitives.h"
#include "DDG4/Geant4InputAction.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Context.h"

#include "G4Event.hh"

using namespace std;
using namespace DD4hep::Simulation;
typedef DD4hep::ReferenceBitMask<int> PropertyMask;

/// Initializing constructor
Geant4EventReader::Geant4EventReader(const std::string& nam) 
  : m_name(nam), m_directAccess(false)
{
}

/// Default destructor
Geant4EventReader::~Geant4EventReader()   {
}

/// Standard constructor
Geant4InputAction::Geant4InputAction(Geant4Context* context, const string& name)
  : Geant4GeneratorAction(context,name), m_reader(0)
{
  declareProperty("Input",          m_input);
  declareProperty("Sync",           m_firstEvent=0);
  declareProperty("Mask",           m_mask = 0);
  declareProperty("MomentumScale",  m_momScale = 1.0);
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
int Geant4InputAction::readParticles(int evt_number, std::vector<Particle*>& particles)  {
  int evid = evt_number + m_firstEvent;
  if ( 0 == m_reader )  {
    if ( m_input.empty() )  {
      throw runtime_error("InputAction: No inoput file declared!");
    }
    string err;
    TypeName tn = TypeName::split(m_input,"|");
    try  {
      m_reader = PluginService::Create<Geant4EventReader*>(tn.first,tn.second);
      if ( 0 == m_reader )   {
	abortRun(issue(evid)+"Error creating reader plugin.",
		 "Failed to create file reader of type %s. Cannot open dataset %s",
		 tn.first.c_str(),tn.second.c_str());
	return 0;
      }
    }
    catch(const exception& e)  {
      err = e.what();
    }
    if ( !err.empty() )  {
      abortRun(issue(evid)+err,"Error when reading file %s",m_input.c_str());
      return 0;
    }
  }
  int status = m_reader->readParticles(evid,particles);
  if ( 0 != status )  {
    abortRun(issue(evid)+"Error when reading file - may be end of file.",
	     "Error when reading file %s",m_input.c_str());
  }
  return status;
}

/// Callback to generate primary particles
void Geant4InputAction::operator()(G4Event* event)   {
  vector<Particle*>         primaries;
  Geant4Event&              evt = context()->event();
  Geant4PrimaryEvent*       prim = evt.extension<Geant4PrimaryEvent>();
  Geant4PrimaryInteraction* inter = new Geant4PrimaryInteraction();

  int result = readParticles(event->GetEventID(),primaries);
  if ( result != 0 )   {    // handle I/O error, but how?
    return;
  }
  prim->add(m_mask, inter);
  // check if there is at least one particle
  if ( primaries.empty() ) return;

  print("+++ Particle interaction with %d generator particles ++++++++++++++++++++++++",
	int(primaries.size()));
  Geant4Vertex* vtx = new Geant4Vertex();
  vtx->x = 0;
  vtx->y = 0;
  vtx->z = 0;
  vtx->time = 0;
  inter->vertices.insert(make_pair(m_mask,vtx));
  // build collection of MCParticles
  for(size_t i=0; i<primaries.size(); ++i )   {
    Geant4ParticleHandle p(primaries[i]);
    const double mom_scale = m_momScale;
    PropertyMask status(p->status);
    p->psx          = mom_scale*p->psx;
    p->psy          = mom_scale*p->psy;
    p->psz          = mom_scale*p->psz;
    if ( p->parents.size() == 0 )  {
      if ( status.isSet(G4PARTICLE_GEN_EMPTY) || status.isSet(G4PARTICLE_GEN_DOCUMENTATION) )
	vtx->in.insert(p->id);  // Beam particles and primary quarks etc.
      else
	vtx->out.insert(p->id); // Stuff, to be given to Geant4 together with daughters
    }
    inter->particles.insert(make_pair(p->id,p));
    p.dump3(outputLevel()-1,name(),"+->");
  }
}
