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

//	Framework include files
#include "DD4hep/VolumeManager.h"
#include "DDG4/Geant4OutputAction.h"
//#include "EDM4hepEventParameters.h"
// Geant4 headers
#include "G4Threading.hh"
#include "G4AutoLock.hh"

#include "DD4hep/Detector.h"
#include <G4Version.hh>

// edm4hep include files
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/SimTrackerHitCollection.h"
#include "edm4hep/CaloHitContributionCollection.h"
#include "edm4hep/SimCalorimeterHitCollection.h"
#include "podio/EventStore.h"
#include "podio/ROOTWriter.h"

#include <typeinfo>
#include <iostream>

using namespace edm4hep ;

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  class ComponentCast;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim {

    class	Geant4ParticleMap;

    /// Base class to output Geant4 event data to EDM4hep
    /**
     *  \author  F.Gaede
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Output2EDM4hep : public Geant4OutputAction  {
    protected:
      podio::EventStore*  m_store;
      podio::ROOTWriter*  m_file;
      int              m_runNo;
      int              m_runNumberOffset;
      int              m_eventNumberOffset;
      std::map< std::string, std::string > m_runHeader;
      std::map< std::string, std::string > m_eventParametersInt;
      std::map< std::string, std::string > m_eventParametersFloat;
      std::map< std::string, std::string > m_eventParametersString;
      bool m_FirstEvent =  true  ;

      /// create the podio collections for the particles and hits
      void createCollections(OutputContext<G4Event>& ctxt) ;
      /// Data conversion interface for MC particles to EDM4hep format
      void saveParticles(Geant4ParticleMap* particles);
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
      void saveEventParameters(podio::EventStore* event, const std::map<std::string, std::string >& parameters);
    };
    
    /// Fill event parameters in EDM4hep event
    template <typename T>
    inline void Geant4Output2EDM4hep::saveEventParameters(podio::EventStore* event, const std::map<std::string, std::string >& parameters)  {
      for(std::map<std::string, std::string >::const_iterator iter = parameters.begin(), endIter = parameters.end() ; iter != endIter ; ++iter)  {
        T parameter;
        std::istringstream iss(iter->second);
        if ( (iss >> parameter).fail() )  {
          printout(FATAL,"saveEventParameters","+++ Event parameter %s: FAILED to convert to type :%s",iter->first.c_str(),typeid(T).name());
          continue;
        }
	printout( WARNING,"saveEventParameters", " not implemented in EDM4hep - not written:   %s  -> %x ",  iter->first,parameter ) ;


//        event->parameters().setValue(iter->first,parameter);
      }
    }

    /// Fill event parameters in EDM4hep event - std::string specialization
    template <>
    inline void Geant4Output2EDM4hep::saveEventParameters<std::string>(podio::EventStore* event, const std::map<std::string, std::string >& parameters)  {
      for(std::map<std::string, std::string >::const_iterator iter = parameters.begin(), endIter = parameters.end() ; iter != endIter ; ++iter)  {
	printout( WARNING, "Geant4Output2EDM4hep","saveEventParameters(): not implemented in EDM4hep - not written:   %s  -> %s ",  iter->first.c_str(),iter->second.c_str() ) ;

//        event->parameters().setValue(iter->first,iter->second);
      }
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

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DD4hep/Detector.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Action.h"

//#include "DDG4/Geant4Output2EDM4hep.h"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4Event.hh"
#include "G4Run.hh"


using namespace dd4hep::sim;
using namespace dd4hep;
using namespace std;
namespace {
  G4Mutex action_mutex=G4MUTEX_INITIALIZER;
}

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4Output2EDM4hep)

/// Standard constructor
Geant4Output2EDM4hep::Geant4Output2EDM4hep(Geant4Context* ctxt, const string& nam)
: Geant4OutputAction(ctxt,nam), m_store(0), m_file(0), m_runNo(0), m_runNumberOffset(0), m_eventNumberOffset(0)
{
  declareProperty("RunHeader", m_runHeader);
  declareProperty("EventParametersInt",    m_eventParametersInt);
  declareProperty("EventParametersFloat",  m_eventParametersFloat);
  declareProperty("EventParametersString", m_eventParametersString);
  declareProperty("RunNumberOffset", m_runNumberOffset);
  declareProperty("EventNumberOffset", m_eventNumberOffset);
  printout( INFO, "Geant4Output2EDM4hep" ," instantiated ..." ) ;
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2EDM4hep::~Geant4Output2EDM4hep()  {
  G4AutoLock protection_lock(&action_mutex);
  if ( m_file )  {
    m_file->finish();
    detail::deletePtr(m_file);
  }
  InstanceCount::decrement(this);
}

// Callback to store the Geant4 run information
void Geant4Output2EDM4hep::beginRun(const G4Run* run)  {
  if ( 0 == m_file && !m_output.empty() )   {
    G4AutoLock protection_lock(&action_mutex);
    m_store = new podio::EventStore ;
    m_file = new podio::ROOTWriter(m_output, m_store);

    printout( INFO, "Geant4Output2EDM4hep" ," opened %s for output", m_output.c_str() ) ;
  }
  
  saveRun(run);
}

/// Callback to store the Geant4 run information
void Geant4Output2EDM4hep::endRun(const G4Run* /*run*/)  {
  // saveRun(run);
}

/// Commit data at end of filling procedure
void Geant4Output2EDM4hep::commit( OutputContext<G4Event>& /* ctxt */)   {
  if ( m_file )   {
    G4AutoLock protection_lock(&action_mutex);
    m_file->writeEvent();
    m_store->clearCollections();
    return;
  }
  except("+++ Failed to write output file. [Stream is not open]");
}

/// Callback to store the Geant4 run information
void Geant4Output2EDM4hep::saveRun(const G4Run* run)  {
  G4AutoLock protection_lock(&action_mutex);

  printout( WARNING, "Geant4Output2EDM4hep" ,"saveRun(): RunHeader not implemented in EDM4hep, nothing written ..." ) ;

  // --- write an edm4hep::RunHeader ---------
//  edm4hep::LCRunHeaderImpl* rh =  new edm4hep::LCRunHeaderImpl;
//  for (std::map< std::string, std::string >::iterator it = m_runHeader.begin(); it != m_runHeader.end(); ++it) {
//    rh->parameters().setValue( it->first, it->second );
//  }
//  m_runNo = m_runNumberOffset > 0 ? m_runNumberOffset + run->GetRunID() : run->GetRunID();
//  rh->parameters().setValue("GEANT4Version", G4Version);
//  rh->parameters().setValue("DD4HEPVersion", versionString());
//  rh->setRunNumber(m_runNo);
//  rh->setDetectorName(context()->detectorDescription().header().name());
//  m_file->writeRunHeader(rh);
}

void Geant4Output2EDM4hep::begin(const G4Event* /* event */)  {

  // put the store into the event w/o ownership transversal
  context()->event().addExtension<podio::EventStore>( m_store, false );
}

/// Data conversion interface for MC particles to EDM4hep format
void Geant4Output2EDM4hep::saveParticles(Geant4ParticleMap* particles)    {
  typedef detail::ReferenceBitMask<const int> PropertyMask;
  typedef Geant4ParticleMap::ParticleMap ParticleMap;
  const ParticleMap& pm = particles->particleMap;
  size_t nparts = pm.size();
  edm4hep::MCParticleCollection* mcpc =
    const_cast<edm4hep::MCParticleCollection*>(
      &m_store->get<edm4hep::MCParticleCollection>("MCParticles"));

  if ( nparts > 0 )  {
    size_t cnt = 0;
    map<int,int> p_ids;
    vector<const Geant4Particle*> p_part(pm.size(),0);
    vector<MCParticle> p_edm4hep(pm.size());
    // First create the particles
    for(ParticleMap::const_iterator i=pm.begin(); i!=pm.end();++i, ++cnt)   {
      int id = (*i).first;
      const Geant4ParticleHandle p = (*i).second;
      PropertyMask mask(p->status);
      //      std::cout << " ********** mcp status : 0x" << std::hex << p->status << ", mask.isSet(G4PARTICLE_GEN_STABLE) x" << std::dec << mask.isSet(G4PARTICLE_GEN_STABLE)  <<std::endl ;
      const G4ParticleDefinition* def = p.definition();
      MCParticle mcp = edm4hep::MCParticle();
      mcp.setPDG(p->pdgID);

      float ps_fa[3] = {float(p->psx/CLHEP::GeV),float(p->psy/CLHEP::GeV),float(p->psz/CLHEP::GeV)};
      mcp.setMomentum( ps_fa );

      float pe_fa[3] = {float(p->pex/CLHEP::GeV),float(p->pey/CLHEP::GeV),float(p->pez/CLHEP::GeV)};
      mcp.setMomentumAtEndpoint( pe_fa );

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
//      std::cout << " ********** mcp genstatus : " << mcp.getGeneratorStatus() << std::endl ;

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

      mcpc->push_back(mcp);
      p_ids[id] = cnt;
      p_part[cnt] = p;
      p_edm4hep[cnt] = mcp;
    }

    // Now establish parent-daughter relationships
    for(size_t i=0, n=p_ids.size(); i<n; ++i)   {
      map<int,int>::iterator k;
      const Geant4Particle* p = p_part[i];
      MCParticle q = p_edm4hep[i];
      const Geant4Particle::Particles& dau = p->daughters;
      for(Geant4Particle::Particles::const_iterator j=dau.begin(); j!=dau.end(); ++j)  {
        int idau = *j;
        if ( (k=p_ids.find(idau)) == p_ids.end() )  {  // Error!!!
          printout(FATAL,"Geant4Conversion","+++ Particle %d: FAILED to find daughter with ID:%d",p->id,idau);
          continue;
        }
        int iqdau = (*k).second;
        MCParticle qdau = p_edm4hep[iqdau];
        qdau.addParent(q);
      }
      const Geant4Particle::Particles& par = p->parents;
      for(Geant4Particle::Particles::const_iterator j=par.begin(); j!=par.end(); ++j)  {
        int ipar = *j; // A parent ID iof -1 means NO parent, because a base of 0 is perfectly leagal!
        if ( ipar>=0 && (k=p_ids.find(ipar)) == p_ids.end() )  {  // Error!!!
          printout(FATAL,"Geant4Conversion","+++ Particle %d: FAILED to find parent with ID:%d",p->id,ipar);
          continue;
        }
        int iqpar = (*k).second;
        MCParticle qpar = p_edm4hep[iqpar];
        q.addParent(qpar);
      }
    }
  }
}

/// Callback to store the Geant4 event
void Geant4Output2EDM4hep::saveEvent(OutputContext<G4Event>& ctxt)  {

  if( m_FirstEvent ){
    createCollections( ctxt ) ;
    m_FirstEvent = false ;
  }

//  podio::EventStore* e = context()->event().extension<podio::EventStore>();
//  EDM4hepEventParameters* parameters = context()->event().extension<EDM4hepEventParameters>(false);
  int runNumber(0), eventNumber(0);
  const int eventNumberOffset(m_eventNumberOffset > 0 ? m_eventNumberOffset : 0);
  const int runNumberOffset(m_runNumberOffset > 0 ? m_runNumberOffset : 0);
  // // Get event number, run number and parameters from extension ...
  // if ( parameters ) {
  //   runNumber = parameters->runNumber() + runNumberOffset;
  //   eventNumber = parameters->eventNumber() + eventNumberOffset;
  // //   EDM4hepEventParameters::copyLCParameters(parameters->eventParameters(),e->parameters());
  // }
  // // ... or from DD4hep framework
  // else {
  runNumber = m_runNo + runNumberOffset;
  eventNumber = ctxt.context->GetEventID() + eventNumberOffset;
  // }
  printout(INFO,"Geant4Output2EDM4hep","+++ Saving EDM4hep event %d run %d.", eventNumber, runNumber);
  // // e->setRunNumber(runNumber);
  // // e->setEventNumber(eventNumber);
  // // e->setDetectorName(context()->detectorDescription().header().name());
  // saveEventParameters<int>(e, m_eventParametersInt);
  // saveEventParameters<float>(e, m_eventParametersFloat);
  // saveEventParameters<std::string>(e, m_eventParametersString);
  podio::EventStore* evt = context()->event().extension<podio::EventStore>();
  Geant4ParticleMap* part_map = context()->event().extension<Geant4ParticleMap>(false);
  if ( part_map )   {
    print("+++ Saving %d EDM4hep particles....",int(part_map->particleMap.size()));
    if ( part_map->particleMap.size() > 0 )  {
      saveParticles(part_map);
    }
  }
}

/// Callback to store each Geant4 hit collection
void Geant4Output2EDM4hep::saveCollection(OutputContext<G4Event>& /*ctxt*/, G4VHitsCollection* collection)  {

  size_t nhits = collection->GetSize();
  std::string colName = collection->GetName();

  printout(DEBUG,"Geant4Output2EDM4hep","+++ Saving EDM4hep collection %s with %d entries.\n",
	   colName.c_str(),int(nhits));

  Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(collection);
  if( coll == nullptr ){
    printout(ERROR, "Geant4Output2EDM4hep" , " no Geant4HitCollection:  %s ", colName.c_str() );
    return ;
  }

  Geant4ParticleMap* pm = context()->event().extension<Geant4ParticleMap>(false);

  edm4hep::MCParticleCollection* mcpc =
    const_cast<edm4hep::MCParticleCollection*>(
      &m_store->get<edm4hep::MCParticleCollection>("MCParticles"));

  //-------------------------------------------------------------------
  if( typeid( Geant4Tracker::Hit ) == coll->type().type  ){

    edm4hep::SimTrackerHitCollection* sthc =
      const_cast<edm4hep::SimTrackerHitCollection*>(&m_store->get<edm4hep::SimTrackerHitCollection>(colName));

    for(unsigned i=0 ; i < nhits ; ++i){
      auto sth = sthc->create() ;

      const Geant4Tracker::Hit* hit = coll->hit(i);
      const Geant4Tracker::Hit::Contribution& t = hit->truth;
      int trackID = pm->particleID(t.trackID);

      auto mcp = mcpc->at(trackID);

      sth.setCellID( hit->cellID ) ;
      sth.setEDep(hit->energyDeposit/CLHEP::GeV);
      sth.setPathLength(hit->length/CLHEP::mm);
      sth.setTime(hit->truth.time/CLHEP::ns);
      sth.setMCParticle(mcp);
      sth.setPosition({hit->position.x()/CLHEP::mm,
		       hit->position.y()/CLHEP::mm,
		       hit->position.z()/CLHEP::mm});
      sth.setMomentum(edm4hep::Vector3f(hit->momentum.x()/CLHEP::GeV,
					hit->momentum.y()/CLHEP::GeV,
					hit->momentum.z()/CLHEP::GeV ));

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
  else if( typeid( Geant4Calorimeter::Hit ) == coll->type().type ){

    edm4hep::SimCalorimeterHitCollection* schc =
      const_cast<edm4hep::SimCalorimeterHitCollection*>(
	&m_store->get<edm4hep::SimCalorimeterHitCollection>(colName));

    colName += "Contributions"  ;

    edm4hep::CaloHitContributionCollection* schcc =
      const_cast<edm4hep::CaloHitContributionCollection*>(
	&m_store->get<edm4hep::CaloHitContributionCollection>(colName));


  } else {

    printout(ERROR, "Geant4Output2EDM4hep" , " unknown type in Geant4HitCollection  %s ",
	     coll->type().type.name() );
  }


}


void Geant4Output2EDM4hep::createCollections(OutputContext<G4Event>& ctxt){

  m_store->create<edm4hep::MCParticleCollection>("MCParticles");
  m_file->registerForWrite("MCParticles");

  const G4Event* evt = ctxt.context ;
  G4HCofThisEvent* hce = evt->GetHCofThisEvent();
  int nCol = hce->GetNumberOfCollections();

  for (int i = 0; i < nCol; ++i) {
    G4VHitsCollection* hc = hce->GetHC(i);
    std::string colName =  hc->GetName() ;
    Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(hc);
    if( coll == nullptr ){
      printout(WARNING, "Geant4Output2EDM4hep" , " no Geant4HitCollection:  %s ", colName.c_str() );
      continue ;
    }

    if( typeid( Geant4Tracker::Hit ) == coll->type().type  ){

      m_store->create<edm4hep::SimTrackerHitCollection>(colName);
      m_file->registerForWrite(colName);
      printout(DEBUG,"Geant4Output2EDM4hep","+++ created collection %s",colName.c_str() );
    }
    else if( typeid( Geant4Calorimeter::Hit ) == coll->type().type ){

      m_store->create<edm4hep::SimCalorimeterHitCollection>(colName);
      m_file->registerForWrite(colName);
      printout(DEBUG,"Geant4Output2EDM4hep""+++ created collection %s",colName.c_str() );

      colName += "Contributions"  ;
      m_store->create<edm4hep::CaloHitContributionCollection>(colName);
      m_file->registerForWrite(colName);
      printout(DEBUG,"Geant4Output2EDM4hep""+++ created collection %s",colName.c_str() );

    } else {

      printout(WARNING, "Geant4Output2EDM4hep" , " unknown type in Geant4HitCollection  %s ",
	       coll->type().type.name() );
    }
  }


}
