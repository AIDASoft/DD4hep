// $Id: Geant4Converter.cpp 603 2013-06-13 21:15:14Z markus.frank $
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

// Framework include files
#define DDG4_MAKE_INSTANTIATIONS
#include "DD4hep/LCDD.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4Particle.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4MonteCarloTruth.h"

// LCIO includes
#include "IMPL/LCCollectionVec.h"
//
#include "IMPL/ClusterImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "UTIL/ILDConf.h"

// Geant4 include files
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"

using namespace std;


/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  typedef ReferenceBitMask<const int> PropertyMask;

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    // Forward declarations
    typedef Geant4Particle Particle;

    /// Data conversion interface for MC particles to LCIO format
    /**
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
			 pair<const Geant4Context*,const Geant4ParticleMap*>,
			 Geant4ParticleMap>::operator()(const arg_t& args)  const {
      typedef MCParticleImpl MYParticleImpl;
      typedef Geant4Conversion<output_t,pair<arg_t::first_type,arg_t::second_type> > _C;
      typedef Geant4ParticleMap::ParticleMap ParticleMap;
      const ParticleMap& pm = args.second->particleMap;
      size_t nparts = pm.size();
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::MCPARTICLE);
      lc_coll->reserve(nparts);
      if ( nparts > 0 )  {
	size_t cnt = 0;
	map<int,int> p_ids;
	vector<const Particle*> p_part(pm.size(),0);
	vector<MYParticleImpl*> p_lcio(pm.size(),0);
	// First create the particles
	for(ParticleMap::const_iterator i=pm.begin(); i!=pm.end();++i, ++cnt)   {
	  int id = (*i).first;
	  const Particle* p = (*i).second;
	  PropertyMask mask(p->status);
	  const G4ParticleDefinition* def = p->definition;
	  MYParticleImpl* q = (MYParticleImpl*)new lcio::MCParticleImpl();
	  q->setPDG(p->pdgID);

	  float ps_fa[3] = { p->psx/GeV, p->psy/GeV, p->psz/GeV } ;  
	  q->setMomentum( ps_fa );

	  double vs_fa[3] = { p->vsx/mm, p->vsy/mm, p->vsz/mm } ;  
	  q->setVertex( vs_fa );

	  double ve_fa[3] = { p->vex/mm, p->vey/mm, p->vez/mm } ;  
	  q->setEndpoint( ve_fa );

	  //q->setMomentum(&p->psx);
	  //q->setVertex(&p->vsx);
	  //q->setEndpoint(&p->vex);


	  q->setTime(p->time/ns);
	  q->setMass(p->mass/GeV);


	  q->setCharge(def ? def->GetPDGCharge() : 0); // Charge(e+) = 1 !

	  // Set generator status
	  //if ( mask.isSet(G4PARTICLE_GEN_EMPTY) ) 
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
	  const Particle* p = p_part[i];
	  MYParticleImpl* q = p_lcio[i];
	  const Particle::Particles& dau = p->daughters;
	  for(Particle::Particles::const_iterator j=dau.begin(); j!=dau.end(); ++j)  {
	    int idau = *j;
	    if ( (k=p_ids.find(idau)) == p_ids.end() )  {  // Error!!!
	      printout(FATAL,"Geant4Conversion","+++ Particle %d: FAILED to find daughter with ID:%d",p->id,idau);
	      continue;
	    }
	    int iqdau = (*k).second;
	    MYParticleImpl* qdau = p_lcio[iqdau];
	    qdau->addParent(q);
	  }
	  const Particle::Particles& par = p->parents;
	  for(Particle::Particles::const_iterator j=par.begin(); j!=par.end(); ++j)  {
	    int ipar = *j; // A parent ID iof -1 means NO parent, because a base of 0 is perfectly leagal!
	    if ( ipar>=0 && (k=p_ids.find(ipar)) == p_ids.end() )  {  // Error!!!
	      printout(FATAL,"Geant4Conversion","+++ Particle %d: FAILED to find parent with ID:%d",p->id,ipar);
	      continue;
	    }
	    int iqpar = (*k).second;
	    MYParticleImpl* qpar = p_lcio[iqpar];
	    q->addParent(qpar);
	  }
	}
      }
      return lc_coll;
    }

    typedef pair<const Geant4Context*,const Geant4ParticleMap*> CONVERSION_ARGS;
    template class Geant4Conversion<lcio::LCCollectionVec,CONVERSION_ARGS>;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,Geant4ParticleMap)
  }    // End namespace Simulation
}      // End namespace DD4hep



