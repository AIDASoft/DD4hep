//==========================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : A.Sailer
//
//==========================================================================

/** \addtogroup Geant4EventReader
 *
 @{
 \package EDM4hepFileReader
 * \brief Plugin to read EDM4hep files
 *
 *
 @}
*/

#include <CLHEP/Units/SystemOfUnits.h>
#include <DDG4/EventParameters.h>
#include <DDG4/Factories.h>
#include <DDG4/Geant4InputAction.h>
#include <edm4hep/MCParticleCollection.h>
#include <podio/Frame.h>
#include <podio/ROOTReader.h>

typedef dd4hep::detail::ReferenceBitMask<int> PropertyMask;


namespace dd4hep::sim {

  // we use the index of the objectID to identify particles
  // we will not support MCParticles from different collections
  using MCPARTICLE_MAP=std::map<int, int>;
  
  /// get the parameters from the input EDM4hep frame and store them in the EventParameters extension
  template <class T=podio::Frame&> void EventParameters::ingestParameters(T const& source) {
    //   for(auto const& key: intKeys) {
    //     m_intValues[key] = std::move(intVec);
    //   }
    //   for(auto const& key: floatKeys) {
    //     m_fltValues[key] = std::move(floatVec);
    //   }
    //   for(auto const& key: stringKeys) {
    //     m_strValues[key] = std::move(stringVec);
    //   }
    // }
  }


  /// Base class to read EDM4hep files
  /**
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class EDM4hepFileReader : public Geant4EventReader {
  protected:
    /// Reference to reader object
    podio::ROOTReader m_reader {};
    std::string m_collectionName;
  public:
    /// Initializing constructor
    EDM4hepFileReader(const std::string& nam);

    /// Read an event and fill a vector of MCParticles.
    virtual EventReaderStatus readParticleCollection(int event_number, const edm4hep::MCParticleCollection** particles);
    virtual EventReaderStatus setParameters(std::map< std::string, std::string >& parameters);

    /// Read an event and fill a vector of MCParticles.
    virtual EventReaderStatus readParticles(int event_number, Vertices& vertices, std::vector<Particle*>& particles);
    /// Read an event and return a LCCollectionVec of MCParticles.
  };




/// Initializing constructor
dd4hep::sim::EDM4hepFileReader::EDM4hepFileReader(const std::string& nam)
: Geant4EventReader(nam)
, m_collectionName("MCParticles")
{
  printout(INFO,"EDM4hepFileReader","Created file reader. Try to open input %s",nam.c_str());
  m_reader.openFile(nam);
  auto categories = m_reader.getAvailableCategories();
  for(auto cat: categories) {
    std::cout << "Category " << cat  << std::endl;
  }

    
  
  m_directAccess = true;
}

  
namespace {
  inline int GET_ENTRY(MCPARTICLE_MAP const& mcparts, int partID)  {
    MCPARTICLE_MAP::const_iterator ip = mcparts.find(partID);
    if ( ip == mcparts.end() )  {
      throw std::runtime_error("Unknown particle identifier look-up!");
    }
    return (*ip).second;
  }
}

  
/// Read an event and fill a vector of MCParticles.
EDM4hepFileReader::EventReaderStatus
EDM4hepFileReader::readParticles(int event_number, Vertices& vertices, std::vector<Particle*>& particles) {

  podio::Frame frame = m_reader.readEntry("events", event_number);
  const auto& primaries = frame.get<edm4hep::MCParticleCollection>(m_collectionName);
  if ( primaries.isValid() ) {
    auto eventNumber = frame.getParameter<int>("EventNumber").value_or(event_number);
    auto runNumber = frame.getParameter<int>("RunNumber").value_or(0);
    printout(INFO,"EDM4hepFileReader","read collection %s from event %d in run %d ", 
             m_collectionName.c_str(), eventNumber, runNumber);
    // Create input event parameters context
    try {
      Geant4Context* ctx = context();
      EventParameters *parameters = new EventParameters();
      parameters->setRunNumber(runNumber);
      parameters->setEventNumber(eventNumber);
      parameters->ingestParameters(frame);
      ctx->event().addExtension<EventParameters>(parameters);
    }
    catch(std::exception &) {}
  } else {
    return EVENT_READER_EOF;
  }

  printout(INFO,"EDM4hepFileReader", "We read the particle collection");
  primaries.print(std::cout);
  std::cout << "is valid after " << primaries.isValid()  << std::endl;
  int NHEP = primaries.size();
  // check if there is at least one particle
  if ( NHEP == 0 ) {
    printout(WARNING,"EDM4hepFileReader", "We have no particles");
    return EVENT_READER_NO_PRIMARIES;
  }
  
  MCPARTICLE_MAP mcparts;
  std::vector<int>  mcpcoll;
  mcpcoll.resize(NHEP);
  for(int i=0; i<NHEP; ++i ) {
    edm4hep::MCParticle p = primaries.at(i);
    mcparts[p.getObjectID().index] = i;
    mcpcoll[i] = p.getObjectID().index;
  }

  // build collection of MCParticles
  for(int i=0; i<NHEP; ++i )   {
    const auto& mcp = primaries.at(i);
    Geant4ParticleHandle p(new Particle(i));
    const auto mom   = mcp.getMomentum();
    const auto vsx   = mcp.getVertex();
    const auto vex   = mcp.getEndpoint();
    const auto spin  = mcp.getSpin();
    const auto color = mcp.getColorFlow();
    const int  pdg   = mcp.getPDG();
    p->pdgID        = pdg;
    p->charge       = int(mcp.getCharge()*3.0);
    p->psx          = mom[0]*CLHEP::GeV;
    p->psy          = mom[1]*CLHEP::GeV;
    p->psz          = mom[2]*CLHEP::GeV;
    p->time         = mcp.getTime()*CLHEP::ns;
    p->properTime   = mcp.getTime()*CLHEP::ns;
    p->vsx          = vsx[0]*CLHEP::mm;
    p->vsy          = vsx[1]*CLHEP::mm;
    p->vsz          = vsx[2]*CLHEP::mm;
    p->vex          = vex[0]*CLHEP::mm;
    p->vey          = vex[1]*CLHEP::mm;
    p->vez          = vex[2]*CLHEP::mm;
    p->process      = 0;
    p->spin[0]      = spin[0];
    p->spin[1]      = spin[1];
    p->spin[2]      = spin[2];
    p->colorFlow[0] = color[0];
    p->colorFlow[1] = color[1];
    p->mass         = mcp.getMass()*CLHEP::GeV;
    const auto par = mcp.getParents(), &dau=mcp.getDaughters();
    for(int num=dau.size(),k=0; k<num; ++k) {
      edm4hep::MCParticle dau_k = dau[k];
      p->daughters.insert(GET_ENTRY(mcparts,dau_k.getObjectID().index));
    }
    for(int num=par.size(),k=0; k<num; ++k) {
      auto par_k = par[k];
      p->parents.insert(GET_ENTRY(mcparts, par_k.getObjectID().index));
    }

    PropertyMask status(p->status);
    int genStatus = mcp.getGeneratorStatus();
    // Copy raw generator status
    p->genStatus = genStatus&G4PARTICLE_GEN_STATUS_MASK;
    m_inputAction->setGeneratorStatus(genStatus, status);

    //fg: we simply add all particles without parents as with their own vertex.
    //    This might include the incoming beam particles, e.g. in
    //    the case of lcio files written with Whizard2, which is slightly odd,
    //    however should be treated correctly in Geant4InputHandling.cpp.
    //    We no longer make an attempt to identify the incoming particles
    //    based on the generator status, as this varies widely with different
    //    generators.

    if ( p->parents.size() == 0 )  {

      Geant4Vertex* vtx = new Geant4Vertex ;
      vertices.emplace_back( vtx );
      vtx->x = p->vsx;
      vtx->y = p->vsy;
      vtx->z = p->vsz;
      vtx->time = p->time;

      vtx->out.insert(p->id) ;
    }

    if ( mcp.isCreatedInSimulation() )       status.set(G4PARTICLE_SIM_CREATED);
    if ( mcp.isBackscatter() )               status.set(G4PARTICLE_SIM_BACKSCATTER);
    if ( mcp.vertexIsNotEndpointOfParent() ) status.set(G4PARTICLE_SIM_PARENT_RADIATED);
    if ( mcp.isDecayedInTracker() )          status.set(G4PARTICLE_SIM_DECAY_TRACKER);
    if ( mcp.isDecayedInCalorimeter() )      status.set(G4PARTICLE_SIM_DECAY_CALO);
    if ( mcp.hasLeftDetector() )             status.set(G4PARTICLE_SIM_LEFT_DETECTOR);
    if ( mcp.isStopped() )                   status.set(G4PARTICLE_SIM_STOPPED);
    if ( mcp.isOverlay() )                   status.set(G4PARTICLE_SIM_OVERLAY);
    particles.emplace_back(p);
  }
  return EVENT_READER_OK;
}


/// Read an event and fill a vector of MCParticles.
Geant4EventReader::EventReaderStatus
dd4hep::sim::EDM4hepFileReader::readParticleCollection(int event_number, const edm4hep::MCParticleCollection** particles) {
  std::cout << "Reading event number " << event_number  << std::endl;

  podio::Frame frame = m_reader.readEntry("events", event_number);

  *particles = (&frame.get<edm4hep::MCParticleCollection>(m_collectionName));
  // std::cout << "collection is valid?: " << particles->isValid()  << std::endl;
  // std::cout << "size " << particles->size()  << std::endl;
  // std::cout << particles->getTypeName()  << std::endl;

  
  (*particles)->print(std::cout);

  if ( (*particles)->isValid() ) {
    auto eventNumber = frame.getParameter<int>("EventNumber").value_or(event_number);
    auto runNumber = frame.getParameter<int>("RunNumber").value_or(0);
    printout(INFO,"EDM4hepFileReader","read collection %s from event %d in run %d ", 
             m_collectionName.c_str(), eventNumber, runNumber);
      
    // Create input event parameters context
    try {
      Geant4Context* ctx = context();
      EventParameters *parameters = new EventParameters();
      parameters->setRunNumber(runNumber);
      parameters->setEventNumber(eventNumber);
      parameters->ingestParameters(frame);
      ctx->event().addExtension<EventParameters>(parameters);
    }
    catch(std::exception &) {}

    return EVENT_READER_OK;
  }
  return EVENT_READER_EOF;
}



/// Set the parameters for the class, in particular the name of the MCParticle
/// list
Geant4EventReader::EventReaderStatus
dd4hep::sim::EDM4hepFileReader::setParameters( std::map< std::string, std::string > & parameters ) {
  _getParameterValue( parameters, "MCParticleCollectionName", m_collectionName, m_collectionName);
  return EVENT_READER_OK;
}


  
} //end dd4hep::sim

DECLARE_GEANT4_EVENT_READER_NS(dd4hep::sim,EDM4hepFileReader)
