//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede, DESY
//
//==========================================================================
#ifndef DD4HEP_DDG4_GEANT4OUTPUT2EDM4hep_H
#define DD4HEP_DDG4_GEANT4OUTPUT2EDM4hep_H

///  Framework include files
#include <DD4hep/Detector.h>
#include <DDG4/EventParameters.h>
#include <DDG4/Geant4OutputAction.h>
#include <DDG4/RunParameters.h>

/// edm4hep include files
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/CaloHitContributionCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>
#include <edm4hep/EDM4hepVersion.h>
/// podio include files
#include <podio/Frame.h>
#if PODIO_VERSION_MAJOR > 0 || (PODIO_VERSION_MAJOR == 0 && PODIO_VERSION_MINOR >= 99)
#include <podio/ROOTWriter.h>
#else
#include <podio/ROOTFrameWriter.h>
#endif
#include <podio/podioVersion.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class ComponentCast;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    class  Geant4ParticleMap;

    /// Base class to output Geant4 event data to EDM4hep
    /**
     *  \author  F.Gaede
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Output2EDM4hep : public Geant4OutputAction  {
    protected:
#if PODIO_VERSION_MAJOR > 0 || (PODIO_VERSION_MAJOR == 0 && PODIO_VERSION_MINOR >= 99)
      using writer_t = podio::ROOTWriter;
#else
      using writer_t = podio::ROOTFrameWriter;
#endif
      using stringmap_t = std::map< std::string, std::string >;
      using trackermap_t = std::map< std::string, edm4hep::SimTrackerHitCollection >;
      using calorimeterpair_t = std::pair< edm4hep::SimCalorimeterHitCollection, edm4hep::CaloHitContributionCollection >;
      using calorimetermap_t = std::map< std::string, calorimeterpair_t >;
      std::unique_ptr<writer_t>     m_file  { };
      podio::Frame                  m_frame { };
      edm4hep::MCParticleCollection m_particles { };
      trackermap_t                  m_trackerHits;
      calorimetermap_t              m_calorimeterHits;
      stringmap_t                   m_runHeader;
      stringmap_t                   m_eventParametersInt;
      stringmap_t                   m_eventParametersFloat;
      stringmap_t                   m_eventParametersString;
      stringmap_t                   m_cellIDEncodingStrings{};
      std::string                   m_section_name      { "events" };
      int                           m_runNo             { 0 };
      int                           m_runNumberOffset   { 0 };
      int                           m_eventNo           { 0 };
      int                           m_eventNumberOffset { 0 };
      bool                          m_filesByRun        { false };
      
      /// Data conversion interface for MC particles to EDM4hep format
      void saveParticles(Geant4ParticleMap* particles);
      /// Store the metadata frame with e.g. the cellID encoding strings
      void saveFileMetaData();
    public:
      /// Standard constructor
      Geant4Output2EDM4hep(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4Output2EDM4hep();
      /// Callback to store the Geant4 run information
      virtual void beginRun(const G4Run* run);
      /// Callback to store the Geant4 run information
      virtual void endRun(const G4Run* run);

      /// Callback to store the Geant4 run information
      virtual void saveRun(const G4Run* run);
      /// Callback to store the Geant4 event
      virtual void saveEvent( OutputContext<G4Event>& ctxt);
      /// Callback to store each Geant4 hit collection
      virtual void saveCollection( OutputContext<G4Event>& ctxt, G4VHitsCollection* collection);
      /// Commit data at end of filling procedure
      virtual void commit( OutputContext<G4Event>& ctxt);

      /// begin-of-event callback - creates EDM4hep event and adds it to the event context
      virtual void begin(const G4Event* event);
    protected:
      /// Fill event parameters in EDM4hep event
      template <typename T>
      void saveEventParameters(const std::map<std::string, std::string >& parameters)   {
        for(const auto& p : parameters)   {
          info("Saving event parameter: %-32s = %s", p.first.c_str(), p.second.c_str());
          m_frame.putParameter(p.first, p.second);
        }
      }
    };
    
    template <> void EventParameters::extractParameters(podio::Frame& frame)   {
      for(auto const& p: this->intParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving event parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
      for(auto const& p: this->fltParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving event parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
      for(auto const& p: this->strParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving event parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
#if podio_VERSION_MAJOR > 0 || podio_VERSION_MINOR > 16 || podio_VERSION_PATCH > 2
      // This functionality is only present in podio > 0.16.2
      for (auto const& p: this->dblParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving event parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
#endif
    }

    template <> void RunParameters::extractParameters(podio::Frame& frame)   {
      for(auto const& p: this->intParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving run parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
      for(auto const& p: this->fltParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving run parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
      for(auto const& p: this->strParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving run parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
#if podio_VERSION_MAJOR > 0 || podio_VERSION_MINOR > 16 || podio_VERSION_PATCH > 2
      // This functionality is only present in podio > 0.16.2
      for (auto const& p: this->dblParameters()) {
        printout(DEBUG, "Geant4OutputEDM4hep", "Saving run parameter: %s", p.first.c_str());
        frame.putParameter(p.first, p.second);
      }
#endif
    }

  }    // End namespace sim
}      // End namespace dd4hep
#endif // DD4HEP_DDG4_GEANT4OUTPUT2EDM4hep_H

//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : F.Gaede, DESY
//
//==========================================================================

/// Framework include files
#include <DD4hep/InstanceCount.h>
#include <DD4hep/VolumeManager.h>

#include <DDG4/Geant4HitCollection.h>
#include <DDG4/Geant4DataConversion.h>
#include <DDG4/Geant4SensDetAction.h>
#include <DDG4/Geant4Context.h>
#include <DDG4/Geant4Particle.h>
#include <DDG4/Geant4Data.h>

///#include <DDG4/Geant4Output2EDM4hep.h>
/// Geant4 headers
#include <G4Threading.hh>
#include <G4AutoLock.hh>
#include <G4Version.hh>
#include <G4ParticleDefinition.hh>
#include <G4VProcess.hh>
#include <G4Event.hh>
#include <G4Run.hh>
/// use the Geant4 units in namespace CLHEP
#include <CLHEP/Units/SystemOfUnits.h>

/// edm4hep include files
#include <edm4hep/EventHeaderCollection.h>

using namespace dd4hep::sim;
using namespace dd4hep;

namespace {
  G4Mutex action_mutex = G4MUTEX_INITIALIZER;
}

#include <DDG4/Factories.h>
DECLARE_GEANT4ACTION(Geant4Output2EDM4hep)

/// Standard constructor
Geant4Output2EDM4hep::Geant4Output2EDM4hep(Geant4Context* ctxt, const std::string& nam)
: Geant4OutputAction(ctxt,nam), m_runNo(0), m_runNumberOffset(0), m_eventNumberOffset(0)
{
  declareProperty("RunHeader",             m_runHeader);
  declareProperty("EventParametersInt",    m_eventParametersInt);
  declareProperty("EventParametersFloat",  m_eventParametersFloat);
  declareProperty("EventParametersString", m_eventParametersString);
  declareProperty("RunNumberOffset",       m_runNumberOffset);
  declareProperty("EventNumberOffset",     m_eventNumberOffset);
  declareProperty("SectionName",           m_section_name);
  declareProperty("FilesByRun",            m_filesByRun);
  info("Writer is now instantiated ..." );
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2EDM4hep::~Geant4Output2EDM4hep()  {
  G4AutoLock protection_lock(&action_mutex);
  m_file.reset();
  InstanceCount::decrement(this);
}

// Callback to store the Geant4 run information
void Geant4Output2EDM4hep::beginRun(const G4Run* run)  {
  G4AutoLock protection_lock(&action_mutex);
  std::string fname = m_output;
  m_runNo = run->GetRunID();
  if ( m_filesByRun )    {
    std::size_t idx = m_output.rfind(".");
    if ( idx != std::string::npos )   {
      fname = m_output.substr(0, idx) + _toString(m_runNo, ".run%08d") + m_output.substr(idx);
    }
  }
  if ( !fname.empty() )   {
#if PODIO_VERSION_MAJOR > 0 || (PODIO_VERSION_MAJOR == 0 && PODIO_VERSION_MINOR >= 99)
    m_file = std::make_unique<podio::ROOTWriter>(fname);
#else
    m_file = std::make_unique<podio::ROOTFrameWriter>(fname);
#endif
    if ( !m_file )   {
      fatal("+++ Failed to open output file: %s", fname.c_str());
    }
    printout( INFO, "Geant4Output2EDM4hep" ,"Opened %s for output", fname.c_str() ) ;
  }
}

/// Callback to store the Geant4 run information
void Geant4Output2EDM4hep::endRun(const G4Run* run)  {
  saveRun(run);
  saveFileMetaData();
  if ( m_file )   {
    m_file->finish();
    m_file.reset();
  }
}

void Geant4Output2EDM4hep::saveFileMetaData() {
  podio::Frame metaFrame{};
  for (const auto& [name, encodingStr] : m_cellIDEncodingStrings) {
    metaFrame.putParameter(name + "__CellIDEncoding", encodingStr);
  }

  m_file->writeFrame(metaFrame, "metadata");
}

/// Commit data at end of filling procedure
void Geant4Output2EDM4hep::commit( OutputContext<G4Event>& /* ctxt */)   {
  if ( m_file )   {
    G4AutoLock protection_lock(&action_mutex);
    m_frame.put( std::move(m_particles), "MCParticles");
    for (auto it = m_trackerHits.begin(); it != m_trackerHits.end(); ++it)   {
      m_frame.put( std::move(it->second), it->first);
    }
    for (auto& [colName, calorimeterHits] : m_calorimeterHits) {
      m_frame.put( std::move(calorimeterHits.first), colName);
      m_frame.put( std::move(calorimeterHits.second), colName + "Contributions");
    }
    m_file->writeFrame(m_frame, m_section_name);
    m_particles.clear();
    m_trackerHits.clear();
    m_calorimeterHits.clear();
    m_frame = {};
    return;
  }
  except("+++ Failed to write output file. [Stream is not open]");
}

/// Callback to store the Geant4 run information
void Geant4Output2EDM4hep::saveRun(const G4Run* run)   {
  G4AutoLock protection_lock(&action_mutex);
  // --- write an edm4hep::RunHeader ---------
  // Runs are just Frames with different contents in EDM4hep / podio. We simply
  // store everything as parameters for now
  podio::Frame runHeader  {};
  for (const auto& [key, value] : m_runHeader)
    runHeader.putParameter(key, value);

  m_runNo = m_runNumberOffset > 0 ? m_runNumberOffset + run->GetRunID() : run->GetRunID();
  runHeader.putParameter("runNumber", m_runNo);
  runHeader.putParameter("GEANT4Version", G4Version);
  runHeader.putParameter("DD4hepVersion", versionString());
  runHeader.putParameter("detectorName", context()->detectorDescription().header().name());

  RunParameters* parameters = context()->run().extension<RunParameters>(false);
  if ( parameters ) {
    parameters->extractParameters(runHeader);
  }

  m_file->writeFrame(runHeader, "runs");
}

void Geant4Output2EDM4hep::begin(const G4Event* event)  {
  /// Create event frame object
  m_eventNo = event->GetEventID();
  m_frame = {};
  m_particles = {};
  m_trackerHits.clear();
  m_calorimeterHits.clear();
}

/// Data conversion interface for MC particles to EDM4hep format
void Geant4Output2EDM4hep::saveParticles(Geant4ParticleMap* particles)    {
  typedef detail::ReferenceBitMask<const int> PropertyMask;
  typedef Geant4ParticleMap::ParticleMap ParticleMap;
  const ParticleMap& pm = particles->particleMap;

  m_particles.clear();
  if ( pm.size() > 0 )  {
    size_t cnt = 0;
    // Mapping of ids in the ParticleMap to indices in the MCParticle collection
    std::map<int,int> p_ids;
    std::vector<const Geant4Particle*> p_part;
    p_part.reserve(pm.size());
    // First create the particles
    for (const auto& iParticle : pm) {
      int id = iParticle.first;
      const Geant4ParticleHandle p = iParticle.second;
      PropertyMask mask(p->status);
      //      std::cout << " ********** mcp status : 0x" << std::hex << p->status << ", mask.isSet(G4PARTICLE_GEN_STABLE) x" << std::dec << mask.isSet(G4PARTICLE_GEN_STABLE)  <<std::endl ;
      const G4ParticleDefinition* def = p.definition();
      auto mcp = m_particles.create();
      mcp.setPDG(p->pdgID);

#if edm4hep_VERSION < EDM4HEP_VERSION(0, 10, 3)
      mcp.setMomentum( {float(p->psx/CLHEP::GeV),float(p->psy/CLHEP::GeV),float(p->psz/CLHEP::GeV)} );
      mcp.setMomentumAtEndpoint( {float(p->pex/CLHEP::GeV),float(p->pey/CLHEP::GeV),float(p->pez/CLHEP::GeV)} );
#else
      mcp.setMomentum( {p->psx/CLHEP::GeV, p->psy/CLHEP::GeV, p->psz/CLHEP::GeV} );
      mcp.setMomentumAtEndpoint( {p->pex/CLHEP::GeV, p->pey/CLHEP::GeV, p->pez/CLHEP::GeV} );
#endif
      double vs_fa[3] = { p->vsx/CLHEP::mm, p->vsy/CLHEP::mm, p->vsz/CLHEP::mm } ;
      mcp.setVertex( vs_fa );

      double ve_fa[3] = { p->vex/CLHEP::mm, p->vey/CLHEP::mm, p->vez/CLHEP::mm } ;
      mcp.setEndpoint( ve_fa );

      mcp.setTime(p->time/CLHEP::ns);
      mcp.setMass(p->mass/CLHEP::GeV);
      mcp.setCharge(def ? def->GetPDGCharge() : 0); // Charge(e+) = 1 !

      // Set generator status
      mcp.setGeneratorStatus(0);
      if( p->genStatus ) {
        mcp.setGeneratorStatus( p->genStatus ) ;
      } else {
        if ( mask.isSet(G4PARTICLE_GEN_STABLE) )             mcp.setGeneratorStatus(1);
        else if ( mask.isSet(G4PARTICLE_GEN_DECAYED) )       mcp.setGeneratorStatus(2);
        else if ( mask.isSet(G4PARTICLE_GEN_DOCUMENTATION) ) mcp.setGeneratorStatus(3);
        else if ( mask.isSet(G4PARTICLE_GEN_BEAM) )          mcp.setGeneratorStatus(4);
        else if ( mask.isSet(G4PARTICLE_GEN_OTHER) )         mcp.setGeneratorStatus(9);
      }

      // Set simulation status
      mcp.setCreatedInSimulation(         mask.isSet(G4PARTICLE_SIM_CREATED) );
      mcp.setBackscatter(                 mask.isSet(G4PARTICLE_SIM_BACKSCATTER) );
      mcp.setVertexIsNotEndpointOfParent( mask.isSet(G4PARTICLE_SIM_PARENT_RADIATED) );
      mcp.setDecayedInTracker(            mask.isSet(G4PARTICLE_SIM_DECAY_TRACKER) );
      mcp.setDecayedInCalorimeter(        mask.isSet(G4PARTICLE_SIM_DECAY_CALO) );
      mcp.setHasLeftDetector(             mask.isSet(G4PARTICLE_SIM_LEFT_DETECTOR) );
      mcp.setStopped(                     mask.isSet(G4PARTICLE_SIM_STOPPED) );
      mcp.setOverlay(                     false );

      //fg: if simstatus !=0 we have to set the generator status to 0:
      if( mcp.isCreatedInSimulation() )
        mcp.setGeneratorStatus( 0 )  ;

      mcp.setSpin(p->spin);
      mcp.setColorFlow(p->colorFlow);

      p_ids[id] = cnt++;
      p_part.push_back(p);
    }

    // Now establish parent-daughter relationships
    for(size_t i=0; i < p_ids.size(); ++i)   {
      const Geant4Particle* p = p_part[i];
      auto q = m_particles[i];

      for (const auto& idau : p->daughters) {
        const auto k = p_ids.find(idau);
        if (k == p_ids.end()) {
          fatal("+++ Particle %d: FAILED to find daughter with ID:%d",p->id,idau);
          continue;
        }
        int iqdau = (*k).second;
        auto qdau = m_particles[iqdau];
        q.addToDaughters(qdau);
      }

      for (const auto& ipar : p->parents) {
        if (ipar >= 0) { // A parent ID of -1 means NO parent, because a base of 0 is perfectly legal
          const auto k = p_ids.find(ipar);
          if (k == p_ids.end()) {
            fatal("+++ Particle %d: FAILED to find parent with ID:%d",p->id,ipar);
            continue;
          }
          int iqpar = (*k).second;
          auto qpar = m_particles[iqpar];
          q.addToParents(qpar);
        }
      }
    }
  }
}

/// Callback to store the Geant4 event
void Geant4Output2EDM4hep::saveEvent(OutputContext<G4Event>& ctxt)  {
  EventParameters* parameters = context()->event().extension<EventParameters>(false);
  int runNumber(0), eventNumber(0);
  const int eventNumberOffset(m_eventNumberOffset > 0 ? m_eventNumberOffset : 0);
  const int runNumberOffset(m_runNumberOffset > 0 ? m_runNumberOffset : 0);
  double eventWeight{0};
  // Get event number, run number and parameters from extension ...
  if ( parameters ) {
    runNumber = parameters->runNumber() + runNumberOffset;
    eventNumber = parameters->eventNumber() + eventNumberOffset;
    parameters->extractParameters(m_frame);
#if podio_VERSION_MAJOR > 0 || podio_VERSION_MINOR > 16 || podio_VERSION_PATCH > 2
    // This functionality is only present in podio > 0.16.2
    eventWeight = m_frame.getParameter<double>("EventWeights");
#endif
  } else { // ... or from DD4hep framework
    runNumber = m_runNo + runNumberOffset;
    eventNumber = ctxt.context->GetEventID() + eventNumberOffset;
  }
  printout(INFO,"Geant4Output2EDM4hep","+++ Saving EDM4hep event %d run %d.", eventNumber, runNumber);

  // this does not compile as create() is we only get a const ref - need to review PODIO EventStore API
  edm4hep::EventHeaderCollection header_collection;
  auto header = header_collection.create();
  header.setRunNumber(runNumber);
  header.setEventNumber(eventNumber);
  header.setWeight(eventWeight);
  //not implemented in EDM4hep ?  header.setDetectorName(context()->detectorDescription().header().name());
  header.setTimeStamp( std::time(nullptr) ) ;
  m_frame.put( std::move(header_collection), "EventHeader");

  saveEventParameters<int>(m_eventParametersInt);
  saveEventParameters<float>(m_eventParametersFloat);
  saveEventParameters<std::string>(m_eventParametersString);

  Geant4ParticleMap* part_map = context()->event().extension<Geant4ParticleMap>(false);
  if ( part_map )   {
    print("+++ Saving %d EDM4hep particles....",int(part_map->particleMap.size()));
    if ( part_map->particleMap.size() > 0 )  {
      saveParticles(part_map);
    }
  }
}

/**
 * Helper struct that can be used together with map::try_emplace to construct
 * the encoding only once per collection (name).
 */
struct LazyEncodingExtraction {
  /// Constructor that does effectively nothing. This will be called in every
  /// try_emplace call
  LazyEncodingExtraction(Geant4HitCollection* coll) : m_coll(coll) {}
  /// Defer the real work to the implicit conversion to std::string that will
  /// only be called if the value is actually emplaced into the map
  operator std::string() const {
    const auto* sd = m_coll->sensitive();
    return dd4hep::sim::Geant4ConversionHelper::encoding(sd->sensitiveDetector());
  }
private:
  Geant4HitCollection* m_coll{nullptr};
};


/// Callback to store each Geant4 hit collection
void Geant4Output2EDM4hep::saveCollection(OutputContext<G4Event>& /*ctxt*/, G4VHitsCollection* collection)  {
  Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(collection);
  std::string colName = collection->GetName();
  if( coll == nullptr ){
    error(" no Geant4HitCollection:  %s ", colName.c_str());
    return ;
  }
  size_t nhits = collection->GetSize();
  Geant4ParticleMap* pm = context()->event().extension<Geant4ParticleMap>(false);
  debug("+++ Saving EDM4hep collection %s with %d entries.", colName.c_str(), int(nhits));

  // Using try_emplace here to only fill this the first time we come across
  m_cellIDEncodingStrings.try_emplace(colName, LazyEncodingExtraction{coll});

  //-------------------------------------------------------------------
  if( typeid( Geant4Tracker::Hit ) == coll->type().type()  ){
    // Create the hit container even if there are no entries!
    auto& hits = m_trackerHits[colName];
    for(unsigned i=0 ; i < nhits ; ++i){
      auto sth = hits->create();
      const Geant4Tracker::Hit* hit = coll->hit(i);
      const Geant4Tracker::Hit::Contribution& t = hit->truth;
      int   trackID   = pm->particleID(t.trackID);
      auto  mcp       = m_particles.at(trackID);
      const auto& mom = hit->momentum;
      const auto& pos = hit->position;
      edm4hep::Vector3f();
      sth.setCellID( hit->cellID ) ;
      sth.setEDep(hit->energyDeposit/CLHEP::GeV);
      sth.setPathLength(hit->length/CLHEP::mm);
      sth.setTime(hit->truth.time/CLHEP::ns);
      sth.setMCParticle(mcp);
      sth.setPosition( {pos.x()/CLHEP::mm, pos.y()/CLHEP::mm, pos.z()/CLHEP::mm} );
      sth.setMomentum( {float(mom.x()/CLHEP::GeV),float(mom.y()/CLHEP::GeV),float(mom.z()/CLHEP::GeV)} );
      auto particleIt = pm->particles().find(trackID);
      if( ( particleIt != pm->particles().end()) ){
        // if the original track ID of the particle is not the same as the
        // original track ID of the hit it was produced by an MCParticle that
        // is no longer stored
        sth.setProducedBySecondary( (particleIt->second->originalG4ID != t.trackID) );
      }
    }
    //-------------------------------------------------------------------
  }
  else if( typeid( Geant4Calorimeter::Hit ) == coll->type().type() ){
    Geant4Sensitive* sd = coll->sensitive();
    int hit_creation_mode = sd->hitCreationMode();
    // Create the hit container even if there are no entries!
    auto& hits = m_calorimeterHits[colName];
    for(unsigned i=0 ; i < nhits ; ++i){
      auto sch = hits.first->create();
      const Geant4Calorimeter::Hit* hit = coll->hit(i);
      const auto& pos = hit->position;
      sch.setCellID( hit->cellID );
      sch.setPosition({float(pos.x()/CLHEP::mm), float(pos.y()/CLHEP::mm), float(pos.z()/CLHEP::mm)});
      sch.setEnergy( hit->energyDeposit/CLHEP::GeV );


      // now add the individual step contributions
      for(auto ci=hit->truth.begin(); ci != hit->truth.end(); ++ci){

        auto sCaloHitCont = hits.second->create();
        sch.addToContributions( sCaloHitCont );

        const Geant4HitData::Contribution& c = *ci;
        int trackID = pm->particleID(c.trackID);
        auto mcp = m_particles.at(trackID);
        sCaloHitCont.setEnergy( c.deposit/CLHEP::GeV );
        sCaloHitCont.setTime( c.time/CLHEP::ns );
        sCaloHitCont.setParticle( mcp );

        if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )     {
          edm4hep::Vector3f p(c.x/CLHEP::mm, c.y/CLHEP::mm, c.z/CLHEP::mm);
          sCaloHitCont.setPDG( c.pdgID );
          sCaloHitCont.setStepPosition( p );
        }
      }
    }
    //-------------------------------------------------------------------
  } else {
    error("+++ unknown type in Geant4HitCollection %s ", coll->type().type().name());
  }
}
