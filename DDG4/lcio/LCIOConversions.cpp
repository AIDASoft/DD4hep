//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#define DDG4_MAKE_INSTANTIATIONS
#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Data.h"

// LCIO includes
#include "lcio.h"
#include "IMPL/LCCollectionVec.h"
//
#include "IMPL/LCEventImpl.h"
#include "IMPL/ClusterImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/MCParticleImpl.h"
//
#include "UTIL/Operators.h"
#include "UTIL/ILDConf.h"

#include <G4SystemOfUnits.hh>

using namespace std;
using namespace lcio ;
//==================================================================================
//
// SimCalorimeterHit:
// ------------------
// LCIO::CHBIT_STEP       If detailed mode set                    | YES if detailed
// LCIO::CHBIT_LONG:      Position is stored                      | YES
// LCIO::CHBIT_ID1:       CellID1 is stored                       | YES
//
//
// SimTrackerHit:
// --------------
// LCIO::THBIT_ID1:       CellID1 is stored (TrackerHit)          | YES
// LCIO::THBIT_MOMENTUM:  Momentum is stored                      | YES if detailed
//
//==================================================================================

/*
 *   dd4hep namespace declaration
 */
namespace dd4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace sim   {

    typedef VolumeManager VolMgr;
    typedef IDDescriptor  IDDescriptor;

    /// Data conversion interface calling lower level explicit convetrers
    /**
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
                         pair<const Geant4Context*,G4VHitsCollection*>,
                         Geant4HitCollection>::operator()(const arg_t& args)  const {
      G4VHitsCollection* c = args.second;
      Geant4HitCollection* coll = dynamic_cast<Geant4HitCollection*>(c);
      if ( coll )  {
        typedef pair<arg_t::first_type,Geant4HitCollection*> _A;
        typedef Geant4Conversion<output_t,_A> _C;
        const _C& cnv= _C::converter(coll->type().type);
        return cnv(_A(args.first,coll));
      }
      throw unrelated_type_error(typeid(Geant4HitCollection),typeid(*c),
                                 "Cannot save the collection entries of:"+c->GetName());
    }

    /// Data conversion interface creating lcio::SimTrackerHitImpl from Geant4Tracker::Hit structures
    /**
     *  This converter is to be used, when the sensitive detectors create fill collections
     *  of type Geant4HitCollection with objects of type **Geant4Tracker::Hit**.
     *  The original objects are untouched and are automatically when the hosting
     *  Geant4HitCollection object is released.
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
                         pair<const Geant4Context*,Geant4HitCollection*>,
                         Geant4Tracker::Hit>::operator()(const arg_t& args)  const   {

      Geant4HitCollection*   coll    = args.second;
      Geant4Sensitive*       sd      = coll->sensitive();
      size_t                 nhits   = coll->GetSize();
      string                 dsc     = encoding(sd->sensitiveDetector());
      Geant4ParticleMap*     pm      = args.first->event().extension<Geant4ParticleMap>();
      lcio::LCEventImpl*     lc_evt  = args.first->event().extension<lcio::LCEventImpl>();
      EVENT::LCCollection*   lc_part = lc_evt->getCollection(lcio::LCIO::MCPARTICLE);
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      UTIL::CellIDEncoder<SimTrackerHit> decoder(dsc,lc_coll);
      int hit_creation_mode = sd->hitCreationMode();

      if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )
        lc_coll->setFlag(UTIL::make_bitset32(LCIO::THBIT_MOMENTUM,LCIO::THBIT_ID1));
      else
        lc_coll->setFlag(LCIO::THBIT_ID1);

      lc_coll->reserve(nhits);
      for(size_t i=0; i<nhits; ++i)   {
        const Geant4Tracker::Hit* hit = coll->hit(i);
        const Geant4Tracker::Hit::Contribution& t = hit->truth;
        int trackID = pm->particleID(t.trackID);
        EVENT::MCParticle* lc_mcp = (EVENT::MCParticle*)lc_part->getElementAt(trackID);
        double pos[3] = {hit->position.x()/CLHEP::mm, hit->position.y()/CLHEP::mm, hit->position.z()/CLHEP::mm};
        lcio::SimTrackerHitImpl* lc_hit = new lcio::SimTrackerHitImpl;
        lc_hit->setCellID0((hit->cellID >>    0         ) & 0xFFFFFFFF);
        lc_hit->setCellID1((hit->cellID >> sizeof(int)*8) & 0xFFFFFFFF);
        lc_hit->setEDep(hit->energyDeposit/CLHEP::GeV);
        lc_hit->setPathLength(hit->length/CLHEP::mm);
        lc_hit->setTime(hit->truth.time/CLHEP::ns);
        lc_hit->setMCParticle(lc_mcp);
        lc_hit->setPosition(pos);
        lc_hit->setMomentum(hit->momentum.x()/CLHEP::GeV,hit->momentum.y()/CLHEP::GeV,hit->momentum.z()/CLHEP::GeV);

#if LCIO_VERSION_GE( 2, 8 )
        auto particleIt = pm->particles().find(trackID);
        if( ( particleIt != pm->particles().end()) ){
	  // if the original track ID of the particle is not the same as the
	  // original track ID of the hit it was produced by an MCParticle that
	  // is no longer stored
	  lc_hit->setProducedBySecondary( (particleIt->second->originalG4ID != t.trackID) );
        }
#endif

        lc_coll->addElement(lc_hit);
      }
      return lc_coll;
    }

    /// Data conversion interface creating lcio::SimCalorimeterHitImpl from Geant4Calorimeter::Hit structures
    /**
     *  This converter is to be used, when the sensitive detectors create fill collections
     *  of type Geant4HitCollection with objects of type **Geant4Calorimeter::Hit**.
     *  The original objects are untouched and are automatically when the hosting
     *  Geant4HitCollection object is released.
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
                         pair<const Geant4Context*,Geant4HitCollection*>,
                         Geant4Calorimeter::Hit>::operator()(const arg_t& args)  const  {
      typedef Geant4HitData::Contributions Contributions;
      Geant4HitCollection*   coll     = args.second;
      Geant4Sensitive*       sd       = coll->sensitive();
      size_t                 nhits    = coll->GetSize();
      string                 dsc      = encoding(sd->sensitiveDetector());
      Geant4ParticleMap*     pm       = args.first->event().extension<Geant4ParticleMap>();
      lcio::LCEventImpl*     lc_evt   = args.first->event().extension<lcio::LCEventImpl>();
      EVENT::LCCollection*   lc_parts = lc_evt->getCollection(lcio::LCIO::MCPARTICLE);
      lcio::LCCollectionVec* lc_coll  = new lcio::LCCollectionVec(lcio::LCIO::SIMCALORIMETERHIT);
      UTIL::CellIDEncoder<SimCalorimeterHit> decoder(dsc,lc_coll);
      int hit_creation_mode = sd->hitCreationMode();

      if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )
        lc_coll->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_STEP,LCIO::CHBIT_ID1));
      else
        lc_coll->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_ID1));

      lc_coll->reserve(nhits);
      if ( sd->hasProperty("HitCreationMode") )  {
        hit_creation_mode = sd->property("HitCreationMode").value<int>();
      }
      for(size_t i=0; i<nhits; ++i)   {
        const Geant4Calorimeter::Hit* hit = coll->hit(i);
        float pos[3] = {float(hit->position.x()/CLHEP::mm), float(hit->position.y()/CLHEP::mm), float(hit->position.z()/CLHEP::mm)};
        lcio::SimCalorimeterHitImpl*  lc_hit = new lcio::SimCalorimeterHitImpl;
        lc_hit->setCellID0((hit->cellID >>    0         ) & 0xFFFFFFFF);
        lc_hit->setCellID1((hit->cellID >> sizeof(int)*8) & 0xFFFFFFFF); // ????
        lc_hit->setPosition(pos);
        ///No! Done when adding particle contrbutions: lc_hit->setEnergy( hit->energyDeposit );
        lc_coll->addElement(lc_hit);
        /// Now add the individual track contributions to the LCIO hit structure
        for(Contributions::const_iterator j=hit->truth.begin(); j!=hit->truth.end(); ++j)   {
          const Geant4HitData::Contribution& c = *j;
          int trackID = pm->particleID(c.trackID);
          EVENT::MCParticle* lc_mcp = (EVENT::MCParticle*)lc_parts->getElementAt(trackID);
          if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )     {
            float contrib_pos[] = {float(c.x/CLHEP::mm), float(c.y/CLHEP::mm), float(c.z/CLHEP::mm)};
#if LCIO_VERSION_GE( 2, 11 )
            lc_hit->addMCParticleContribution(lc_mcp, c.deposit/CLHEP::GeV, c.time/CLHEP::ns, c.length/CLHEP::mm, c.pdgID, contrib_pos);
#else
            lc_hit->addMCParticleContribution(lc_mcp, c.deposit/CLHEP::GeV, c.time/CLHEP::ns, c.pdgID, contrib_pos);
#endif
          }
          else    {
            lc_hit->addMCParticleContribution(lc_mcp, c.deposit/CLHEP::GeV, c.time/CLHEP::ns);
          }
        }
      }
      return lc_coll;
    }

    template <typename T>
    lcio::LCCollectionVec* moveEntries(Geant4HitCollection* coll,
                                       lcio::LCCollectionVec* lc_coll)
    {
      size_t nhits = coll->GetSize();
      lc_coll->reserve(nhits);
      for(size_t i=0; i<nhits; ++i)   {
        Geant4HitWrapper& wrap = coll->hit(i);
        T* lc_hit = wrap;
        wrap.release();  // Now we have ownership!
        lc_coll->addElement(lc_hit);
      }
      coll->clear(); // Since the collection now only contains NULL pointers, better clear it!
      return lc_coll;
    }

    /// Data conversion interface moving lcio::SimTrackerHitImpl objects from a Geant4HitCollection to a LCCollectionVec
    /**
     *  This converter is to be used, when the sensitive detectors create fill collections
     *  of type Geant4HitCollection with objects of type **lcio::SimTrackerHitImpl**.
     *  The ownership of the original Geant4HitCollection is released and moved to
     *  the newly created lcio::LCCollectionVec container.
     *  Finally the original hits collection is cleared.
     *
     *  Note: This conversion is INTRUSIVE and CLEARS the original collection!
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
                         pair<const Geant4Context*,Geant4HitCollection*>,
                         lcio::SimTrackerHitImpl>::operator()(const arg_t& args)  const
    {
      Geant4Sensitive* sd  = args.second->sensitive();
      string           dsc = encoding(sd->sensitiveDetector());
      output_t*        lc  = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      int hit_creation_mode = sd->hitCreationMode();

      if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )
        lc->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_STEP,LCIO::CHBIT_ID1));
      else
        lc->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_ID1));
      UTIL::CellIDEncoder<SimTrackerHit> decoder(dsc,lc);
      return moveEntries<lcio::SimTrackerHitImpl>(args.second,lc);
    }

    /// Data conversion interface moving lcio::SimCalorimeterHitImpl objects from a Geant4HitCollection to a LCCollectionVec
    /**
     *  This converter is to be used, when the sensitive detectors create fill collections
     *  of type Geant4HitCollection with objects of type **lcio::SimCalorimeterHitImpl**.
     *  The ownership of the original Geant4HitCollection is released and moved to
     *  the newly created lcio::LCCollectionVec container.
     *  Finally the original hits collection is cleared.
     *
     *  Note: This conversion is INTRUSIVE and CLEARS the original collection!
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
                         pair<const Geant4Context*,Geant4HitCollection*>,
                         lcio::SimCalorimeterHitImpl>::operator()(const arg_t& args)  const
    {
      Geant4Sensitive*  sd  = args.second->sensitive();
      //string          dsc = encoding(args.second->sensitive()->sensitiveDetector());
      output_t*         lc  = new lcio::LCCollectionVec(lcio::LCIO::SIMCALORIMETERHIT);
      int hit_creation_mode = sd->hitCreationMode();

      if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )
        lc->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_STEP,LCIO::CHBIT_ID1));
      else
        lc->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_ID1));
      return moveEntries<tag_t>(args.second,lc);
    }

    /// Data conversion interface moving lcio::ClusterImpl objects from a Geant4HitCollection to a LCCollectionVec
    /** Same comments apply as for the data mover for lcio::SimCalorimeterHitImpl and lcio::SimTrackerHitImpl
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
                         pair<const Geant4Context*,Geant4HitCollection*>,
                         lcio::ClusterImpl>::operator()(const arg_t& args)  const
    {
      output_t* lc = new lcio::LCCollectionVec(lcio::LCIO::CLUSTER);
      return moveEntries<tag_t>(args.second,lc);
    }

    typedef pair<const Geant4Context*,G4VHitsCollection*> RAW_CONVERSION_ARGS;
    typedef pair<const Geant4Context*,Geant4HitCollection*> CONVERSION_ARGS;
    template class Geant4Conversion<lcio::LCCollectionVec,RAW_CONVERSION_ARGS>;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,RAW_CONVERSION_ARGS,Geant4HitCollection)

    template class Geant4Conversion<lcio::LCCollectionVec,CONVERSION_ARGS>;
    // Hit converters for simple Geant4Data objects
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,Geant4Tracker::Hit)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,Geant4Calorimeter::Hit)
    // Hit converters for standard LCIO objects
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::SimTrackerHitImpl)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::SimCalorimeterHitImpl)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::ClusterImpl)
  }    // End namespace sim
}      // End namespace dd4hep



