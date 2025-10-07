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
#include <DDG4/FileParameters.h>
#include <DDG4/Geant4InputAction.h>
#include <DDG4/RunParameters.h>

#include <edm4hep/Constants.h>
#include <edm4hep/EventHeaderCollection.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/EDM4hepVersion.h>
#if EDM4HEP_BUILD_VERSION >= EDM4HEP_VERSION(0, 99, 3)
#include <edm4hep/GeneratorEventParametersCollection.h>
#endif

#include <podio/Frame.h>
#include <podio/Reader.h>

typedef dd4hep::detail::ReferenceBitMask<int> PropertyMask;


namespace dd4hep::sim {

  // we use the index of the objectID to identify particles
  // we will not support MCParticles from different collections
  using MCPARTICLE_MAP=std::map<int, int>;

  /// get the parameters from the GenericParameters of the input EDM4hep frame and store them in the EventParameters
  /// extension
  template <class T=podio::GenericParameters> void EventParameters::ingestParameters(T const& source) {
    for(auto const& key: source.template getKeys<int>()) {
      m_intValues[key] = source.template get<std::vector<int>>(key).value();
    }
    for(auto const& key: source.template getKeys<float>()) {
      m_fltValues[key] = source.template get<std::vector<float>>(key).value();
    }
    for(auto const& key: source.template getKeys<double>()) {
      m_dblValues[key] = source.template get<std::vector<double>>(key).value();
    }
    for(auto const& key: source.template getKeys<std::string>()) {
      m_strValues[key] = source.template get<std::vector<std::string>>(key).value();
    }
  }

  /// get the parameters from the GenericParameters of the input EDM4hep run frame and store them in the RunParameters
  /// extension
  template <class T=podio::GenericParameters> void RunParameters::ingestParameters(T const& source) {
    for(auto const& key: source.template getKeys<int>()) {
      m_intValues[key] = source.template get<std::vector<int>>(key).value();
    }
    for(auto const& key: source.template getKeys<float>()) {
      m_fltValues[key] = source.template get<std::vector<float>>(key).value();
    }
    for(auto const& key: source.template getKeys<double>()) {
      m_dblValues[key] = source.template get<std::vector<double>>(key).value();
    }
    for(auto const& key: source.template getKeys<std::string>()) {
      m_strValues[key] = source.template get<std::vector<std::string>>(key).value();
    }
  }

  template <class T=podio::GenericParameters> void FileParameters::ingestParameters(T const& source) {
    for(auto const& key: source.template getKeys<int>()) {
      m_intValues[key] = source.template get<std::vector<int>>(key).value();
    }
    for(auto const& key: source.template getKeys<float>()) {
      m_fltValues[key] = source.template get<std::vector<float>>(key).value();
    }
    for(auto const& key: source.template getKeys<double>()) {
      m_dblValues[key] = source.template get<std::vector<double>>(key).value();
    }
    for(auto const& key: source.template getKeys<std::string>()) {
      m_strValues[key] = source.template get<std::vector<std::string>>(key).value();
    }
  }

  /// Class to read EDM4hep files
  /**
   *  \version 1.0
   *  \ingroup DD4HEP_SIMULATION
   */
  class EDM4hepFileReader : public Geant4EventReader {
  protected:
    /// Reference to reader object
    podio::Reader m_reader;
    /// Name of the MCParticle collection to read
    std::string m_collectionName;
    /// Name of the EventHeader collection to read
    std::string m_eventHeaderCollectionName;

  public:
    /// Initializing constructor
    EDM4hepFileReader(const std::string& nam);

    /// Read parameters set for this action
    virtual EventReaderStatus setParameters(std::map< std::string, std::string >& parameters);

    /// Read an event and fill a vector of MCParticles.
    virtual EventReaderStatus readParticles(int event_number, Vertices& vertices, std::vector<Particle*>& particles);
    /// Call to register the run parameters
    void registerRunParameters();

  };

  /// Initializing constructor
  dd4hep::sim::EDM4hepFileReader::EDM4hepFileReader(const std::string& nam)
    : Geant4EventReader(nam)
    , m_reader(podio::makeReader(nam))
    , m_collectionName("MCParticles")
    , m_eventHeaderCollectionName("EventHeader")
  {
    printout(INFO,"EDM4hepFileReader","Created file reader. Try to open input %s",nam.c_str());
    m_directAccess = true;
  }

  void EDM4hepFileReader::registerRunParameters() {
    try {
      // get RunParameters or create new if not existent yet
      auto* parameters = context()->run().extension<RunParameters>(false);
      if (!parameters) {
        parameters = new RunParameters();
        context()->run().addExtension<RunParameters>(parameters);
      }
      try {
        podio::Frame runFrame = m_reader.readFrame("runs", 0);
        parameters->ingestParameters(runFrame.getParameters());
      } catch (std::runtime_error& e) {
        // we ignore if we do not have runs information
      } catch(std::invalid_argument&) {
        // we ignore if we do not have runs information
      }
    } catch(std::exception &e) {
      printout(ERROR,"EDM4hepFileReader::registerRunParameters","Failed to register run parameters: %s", e.what());
    }

    try {
      auto *fileParameters = new FileParameters();
      try {
        podio::Frame metaFrame = m_reader.readFrame("metadata", 0);
        fileParameters->ingestParameters(metaFrame.getParameters());
      } catch (std::runtime_error& e) {
        // we ignore if we do not have metadata information
      } catch(std::invalid_argument&) {
        // we ignore if we do not have metadata information
      }
      context()->run().addExtension<FileParameters>(fileParameters);
    } catch(std::exception &e) {
      printout(ERROR,"EDM4hepFileReader::registerRunParameters","Failed to register file parameters: %s", e.what());
    }
  }

  namespace {
    /// Helper function to look up MCParticles from mapping
    inline int GET_ENTRY(MCPARTICLE_MAP const& mcparts, int partID)  {
      MCPARTICLE_MAP::const_iterator ip = mcparts.find(partID);
      if ( ip == mcparts.end() )  {
        throw std::runtime_error("Unknown particle identifier look-up!");
      }
      return (*ip).second;
    }
  }

  /// Read an event and fill a vector of MCParticles
  EDM4hepFileReader::EventReaderStatus
  EDM4hepFileReader::readParticles(int event_number, Vertices& vertices, std::vector<Particle*>& particles) {
    m_currEvent = event_number;
    podio::Frame frame = m_reader.readFrame("events", event_number);
    const auto& primaries = frame.get<edm4hep::MCParticleCollection>(m_collectionName);
    int eventNumber = event_number, runNumber = 0;
    if (primaries.isValid()) {
      //Read the event header collection and add it to the context as an extension
      const auto& eventHeaderCollection = frame.get<edm4hep::EventHeaderCollection>(m_eventHeaderCollectionName);
      if(eventHeaderCollection.isValid() && eventHeaderCollection.size() == 1){
        const auto& eh = eventHeaderCollection.at(0);
        eventNumber = eh.getEventNumber();
        runNumber = eh.getRunNumber();
        try {
          Geant4Context* ctx = context();
          ctx->event().addExtension<edm4hep::MutableEventHeader>(new edm4hep::MutableEventHeader(eh.clone()));
        } catch(std::exception& ) {}
        // Create input event parameters context
        try {
          Geant4Context* ctx = context();
          EventParameters *parameters = new EventParameters();
          parameters->setRunNumber(runNumber);
          parameters->setEventNumber(eventNumber);
          parameters->ingestParameters(frame.getParameters());
          ctx->event().addExtension<EventParameters>(parameters);
        } catch(std::exception &) {}
      } else {
        printout(WARNING,"EDM4hepFileReader","No EventHeader collection found or too many event headers!");
        try {
          Geant4Context* ctx = context();
          EventParameters *parameters = new EventParameters();
          parameters->setRunNumber(0);
          parameters->setEventNumber(event_number);
          parameters->ingestParameters(frame.getParameters());
          ctx->event().addExtension<EventParameters>(parameters);
        } catch(std::exception &) {}
      }
#if EDM4HEP_BUILD_VERSION >= EDM4HEP_VERSION(0, 99, 3)
      // Attach the GeneratorEventParameters if they are available
      const auto &genEvtParameters = frame.get<edm4hep::GeneratorEventParametersCollection>(edm4hep::labels::GeneratorEventParameters);
      if (genEvtParameters.isValid()) {
        if (genEvtParameters.size() >= 1) {
          const auto genParams = genEvtParameters[0];
          try {
            auto *ctx = context();
            ctx->event().addExtension(new edm4hep::MutableGeneratorEventParameters(genParams.clone()));
          } catch (std::exception &) {}
        }
        if (genEvtParameters.size() > 1) {
          printout(WARNING, "EDM4hepFileReader", "Multiple GeneratorEventParameters found in input file. Ignoring all but one!");
        }
      } else {
        printout(DEBUG, "EDM4hepFileReader", "No GeneratorEventParameters found in input file");
      }
#endif
      printout(INFO,"EDM4hepFileReader","read collection %s from event %d in run %d ",
               m_collectionName.c_str(), eventNumber, runNumber);

    } else {
      return EVENT_READER_EOF;
    }

    printout(INFO,"EDM4hepFileReader", "We read the particle collection");
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
      const int  pdg   = mcp.getPDG();
#ifdef EDM4HEP_MCPARTICLE_HAS_HELICITY
      p->spin[0] = 0;
      p->spin[1] = 0;
      p->spin[2] = mcp.getHelicity();
#else
      const auto spin  = mcp.getSpin();
      p->spin[0]      = spin[0];
      p->spin[1]      = spin[1];
      p->spin[2]      = spin[2];
#endif
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
      p->colorFlow[0] = 0;
      p->colorFlow[1] = 0;
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
      if(m_inputAction) {
        // in some tests we do not set up the inputAction
        m_inputAction->setGeneratorStatus(genStatus, status);
      }

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

  /// Set the parameters for the class, in particular the name of the MCParticle
  /// list
  Geant4EventReader::EventReaderStatus
  dd4hep::sim::EDM4hepFileReader::setParameters( std::map< std::string, std::string > & parameters ) {
    _getParameterValue(parameters, "MCParticleCollectionName", m_collectionName, m_collectionName);
    _getParameterValue(parameters, "EventHeaderCollectionName", m_eventHeaderCollectionName, m_eventHeaderCollectionName);
    return EVENT_READER_OK;
  }

} //end dd4hep::sim

DECLARE_GEANT4_EVENT_READER_NS(dd4hep::sim,EDM4hepFileReader)
