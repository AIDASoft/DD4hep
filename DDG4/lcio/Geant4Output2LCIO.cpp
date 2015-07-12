// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

#ifndef DD4HEP_DDG4_GEANT4OUTPUT2LCIO_H
#define DD4HEP_DDG4_GEANT4OUTPUT2LCIO_H

// Framework include files
#include "DD4hep/VolumeManager.h"
#include "DDG4/Geant4OutputAction.h"

// lcio include files
#include "lcio.h"
#include "IO/LCWriter.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCCollectionVec.h"

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  class ComponentCast;

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace Simulation {

    class Geant4ParticleMap;

    /// Base class to output Geant4 event data to media
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class Geant4Output2LCIO : public Geant4OutputAction  {
    protected:
      lcio::LCWriter*  m_file;
      int              m_runNo;

      /// Data conversion interface for MC particles to LCIO format
      lcio::LCCollectionVec* saveParticles(Geant4ParticleMap* particles);
    public:
      /// Standard constructor
      Geant4Output2LCIO(Geant4Context* ctxt, const std::string& nam);
      /// Default destructor
      virtual ~Geant4Output2LCIO();
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

      /// begin-of-event callback - creates LCIO event and adds it to the event context
      virtual void begin(const G4Event* event);
    };

  }    // End namespace Simulation
}      // End namespace DD4hep
#endif // DD4HEP_DDG4_GEANT4OUTPUT2LCIO_H

// $Id: Handle.h 570 2013-05-17 07:47:11Z markus.frank $
//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation européenne pour la Recherche nucléaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/InstanceCount.h"
#include "DD4hep/LCDD.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4Data.h"

//#include "DDG4/Geant4Output2LCIO.h"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4Event.hh"
#include "G4Run.hh"

// LCIO include files
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/ClusterImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "UTIL/ILDConf.h"

using namespace DD4hep::Simulation;
using namespace DD4hep;
using namespace std;

#include "DDG4/Factories.h"
DECLARE_GEANT4ACTION(Geant4Output2LCIO)

/// Standard constructor
Geant4Output2LCIO::Geant4Output2LCIO(Geant4Context* ctxt, const string& nam)
: Geant4OutputAction(ctxt,nam), m_file(0), m_runNo(0)
{
  InstanceCount::increment(this);
}

/// Default destructor
Geant4Output2LCIO::~Geant4Output2LCIO()  {
  if ( m_file )  {
    m_file->close();
    deletePtr(m_file);
  }
  InstanceCount::decrement(this);
}

// Callback to store the Geant4 run information
void Geant4Output2LCIO::beginRun(const G4Run* )  {
  if ( 0 == m_file && !m_output.empty() )   {
    m_file = lcio::LCFactory::getInstance()->createLCWriter();
    m_file->open(m_output,lcio::LCIO::WRITE_NEW);
  }
}

/// Callback to store the Geant4 run information
void Geant4Output2LCIO::endRun(const G4Run* )  {
}

/// Commit data at end of filling procedure
void Geant4Output2LCIO::commit( OutputContext<G4Event>& /* ctxt */)   {
  lcio::LCEventImpl* e = context()->event().extension<lcio::LCEventImpl>();
  m_file->writeEvent(e);
  //  std::cout << " ########### Geant4Output2LCIO::commit() : writing LCIO event to file .... "  << std::endl ;
}

/// Callback to store the Geant4 run information
void Geant4Output2LCIO::saveRun(const G4Run* run)  {
  // --- write an lcio::RunHeader ---------
  lcio::LCRunHeaderImpl* rh =  new lcio::LCRunHeaderImpl;
  rh->setRunNumber(m_runNo=run->GetRunID());
  rh->setDetectorName(context()->lcdd().header().name());
  m_file->writeRunHeader(rh);
}

void Geant4Output2LCIO::begin(const G4Event* /* event */)  {
  lcio::LCEventImpl* e  = new lcio::LCEventImpl;
  //fg: here the event context takes ownership and
  //    deletes the event in the end
  context()->event().addExtension<lcio::LCEventImpl>( e );
}

/// Data conversion interface for MC particles to LCIO format
lcio::LCCollectionVec* Geant4Output2LCIO::saveParticles(Geant4ParticleMap* particles)    {
  typedef ReferenceBitMask<const int> PropertyMask;
  typedef Geant4ParticleMap::ParticleMap ParticleMap;
  const ParticleMap& pm = particles->particleMap;
  size_t nparts = pm.size();
  lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::MCPARTICLE);
  lc_coll->reserve(nparts);
  if ( nparts > 0 )  {
    size_t cnt = 0;
    map<int,int> p_ids;
    vector<const Geant4Particle*> p_part(pm.size(),0);
    vector<MCParticleImpl*> p_lcio(pm.size(),0);
    // First create the particles
    for(ParticleMap::const_iterator i=pm.begin(); i!=pm.end();++i, ++cnt)   {
      int id = (*i).first;
      const Geant4ParticleHandle p = (*i).second;
      PropertyMask mask(p->status);
      const G4ParticleDefinition* def = p.definition();
      MCParticleImpl* q = new lcio::MCParticleImpl();
      q->setPDG(p->pdgID);

      float ps_fa[3] = {float(p->psx/CLHEP::GeV),float(p->psy/CLHEP::GeV),float(p->psz/CLHEP::GeV)};
      q->setMomentum( ps_fa );

      double vs_fa[3] = { p->vsx/CLHEP::mm, p->vsy/CLHEP::mm, p->vsz/CLHEP::mm } ;
      q->setVertex( vs_fa );

      double ve_fa[3] = { p->vex/CLHEP::mm, p->vey/CLHEP::mm, p->vez/CLHEP::mm } ;
      q->setEndpoint( ve_fa );

      q->setTime(p->time/CLHEP::ns);
      q->setMass(p->mass/CLHEP::GeV);
      q->setCharge(def ? def->GetPDGCharge() : 0); // Charge(e+) = 1 !

      // Set generator status
      q->setGeneratorStatus(0);
      if ( mask.isSet(G4PARTICLE_GEN_STABLE) ) q->setGeneratorStatus(1);
      else if ( mask.isSet(G4PARTICLE_GEN_DECAYED) ) q->setGeneratorStatus(2);
      else if ( mask.isSet(G4PARTICLE_GEN_DOCUMENTATION) ) q->setGeneratorStatus(3);

      // Set simulation status
      q->setSimulatorStatus( 0 ) ;
      q->setCreatedInSimulation(         mask.isSet(G4PARTICLE_SIM_CREATED) );
      q->setBackscatter(                 mask.isSet(G4PARTICLE_SIM_BACKSCATTER) );
      q->setVertexIsNotEndpointOfParent( mask.isSet(G4PARTICLE_SIM_PARENT_RADIATED) );
      q->setDecayedInTracker(            mask.isSet(G4PARTICLE_SIM_DECAY_TRACKER) );
      q->setDecayedInCalorimeter(        mask.isSet(G4PARTICLE_SIM_DECAY_CALO) );
      q->setHasLeftDetector(             mask.isSet(G4PARTICLE_SIM_LEFT_DETECTOR) );
      q->setStopped(                     mask.isSet(G4PARTICLE_SIM_STOPPED) );
      q->setOverlay(                     false );

      //fg: if simstatus !=0 we have to set the generator status to 0:
      if( q->getSimulatorStatus() != 0 )
        q->setGeneratorStatus( 0 )  ;

      q->setSpin(p->spin);
      q->setColorFlow(p->colorFlow);

      lc_coll->addElement(q);
      p_ids[id] = cnt;
      p_part[cnt] = p;
      p_lcio[cnt] = q;
    }

    // Now establish parent-daughter relationships
    for(size_t i=0, n=p_ids.size(); i<n; ++i)   {
      map<int,int>::iterator k;
      const Geant4Particle* p = p_part[i];
      MCParticleImpl* q = p_lcio[i];
      const Geant4Particle::Particles& dau = p->daughters;
      for(Geant4Particle::Particles::const_iterator j=dau.begin(); j!=dau.end(); ++j)  {
        int idau = *j;
        if ( (k=p_ids.find(idau)) == p_ids.end() )  {  // Error!!!
          printout(FATAL,"Geant4Conversion","+++ Particle %d: FAILED to find daughter with ID:%d",p->id,idau);
          continue;
        }
        int iqdau = (*k).second;
        MCParticleImpl* qdau = p_lcio[iqdau];
        qdau->addParent(q);
      }
      const Geant4Particle::Particles& par = p->parents;
      for(Geant4Particle::Particles::const_iterator j=par.begin(); j!=par.end(); ++j)  {
        int ipar = *j; // A parent ID iof -1 means NO parent, because a base of 0 is perfectly leagal!
        if ( ipar>=0 && (k=p_ids.find(ipar)) == p_ids.end() )  {  // Error!!!
          printout(FATAL,"Geant4Conversion","+++ Particle %d: FAILED to find parent with ID:%d",p->id,ipar);
          continue;
        }
        int iqpar = (*k).second;
        MCParticleImpl* qpar = p_lcio[iqpar];
        q->addParent(qpar);
      }
    }
  }
  return lc_coll;
}

/// Callback to store the Geant4 event
void Geant4Output2LCIO::saveEvent(OutputContext<G4Event>& ctxt)  {
  lcio::LCEventImpl* e = context()->event().extension<lcio::LCEventImpl>();
  e->setRunNumber(m_runNo);
  e->setEventNumber(ctxt.context->GetEventID());
  e->setDetectorName(context()->lcdd().header().name());
  e->setRunNumber(m_runNo);
  lcio::LCEventImpl* evt = context()->event().extension<lcio::LCEventImpl>();
  Geant4ParticleMap* part_map = context()->event().extension<Geant4ParticleMap>(false);
  if ( part_map )   {
    print("+++ Saving %d LCIO particles....",int(part_map->particleMap.size()));
    if ( part_map->particleMap.size() > 0 )  {
      lcio::LCCollectionVec* col = saveParticles(part_map);
      evt->addCollection(col,lcio::LCIO::MCPARTICLE);
    }
  }
}

/// Callback to store each Geant4 hit collection
void Geant4Output2LCIO::saveCollection(OutputContext<G4Event>& /* ctxt */, G4VHitsCollection* collection)  {
  size_t nhits = collection->GetSize();
  std::string hc_nam = collection->GetName();
  print("+++ Saving LCIO collection %s with %d entries....",hc_nam.c_str(),int(nhits));
  typedef pair<const Geant4Context*,G4VHitsCollection*> _Args;
  typedef Geant4Conversion<lcio::LCCollectionVec,_Args> _C;
  const _C& cnv = _C::converter(typeid(Geant4HitCollection));
  lcio::LCEventImpl* evt = context()->event().extension<lcio::LCEventImpl>();
  lcio::LCCollectionVec* col = cnv(_Args(context(),collection));
  evt->addCollection(col,hc_nam);
}
