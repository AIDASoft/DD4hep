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
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4SensDetAction.h"
#include "DDG4/Geant4Context.h"
#include "DDG4/Geant4Primary.h"
#include "DDG4/Geant4Data.h"

// LCIO includes
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
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    typedef Geometry::VolumeManager VolMgr;
    typedef Geometry::IDDescriptor  IDDescriptor;

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
        lc_hit->setCellID0((hit->cellID >>    0       ) & 0xFFFFFFFF);
        lc_hit->setCellID1((hit->cellID >> sizeof(int)) & 0xFFFFFFFF);
        lc_hit->setEDep(hit->energyDeposit/CLHEP::GeV);
        lc_hit->setPathLength(hit->length/CLHEP::mm);
        lc_hit->setTime(hit->truth.time/CLHEP::ns);
        lc_hit->setMCParticle(lc_mcp);
        lc_hit->setPosition(pos);
        lc_hit->setMomentum(hit->momentum.x()/CLHEP::GeV,hit->momentum.y()/CLHEP::GeV,hit->momentum.z()/CLHEP::GeV);
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
        float pos[3] = {float(hit->position.x()/mm), float(hit->position.y()/mm), float(hit->position.z()/mm)};
        lcio::SimCalorimeterHitImpl*  lc_hit = new lcio::SimCalorimeterHitImpl;
        lc_hit->setCellID0((hit->cellID >>    0       ) & 0xFFFFFFFF);
        lc_hit->setCellID1((hit->cellID >> sizeof(int)) & 0xFFFFFFFF); // ????
        lc_hit->setPosition(pos);
        ///No! Done when adding particle contrbutions: lc_hit->setEnergy( hit->energyDeposit );
        lc_coll->addElement(lc_hit);
        /// Now add the individual track contributions to the LCIO hit structure
        for(Contributions::const_iterator j=hit->truth.begin(); j!=hit->truth.end(); ++j)   {
          const Geant4HitData::Contribution& c = *j;
          int trackID = pm->particleID(c.trackID);
          float pos[] = {float(c.x/mm), float(c.y/mm), float(c.z/mm)};
          EVENT::MCParticle* lc_mcp = (EVENT::MCParticle*)lc_parts->getElementAt(trackID);
          if ( hit_creation_mode == Geant4Sensitive::DETAILED_MODE )
            lc_hit->addMCParticleContribution(lc_mcp, c.deposit/GeV, c.time/ns, lc_mcp->getPDG(), pos);
          else
            lc_hit->addMCParticleContribution(lc_mcp, c.deposit/GeV, c.time/ns);
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
      Geant4Sensitive* sd  = args.second->sensitive();
      string           dsc = encoding(args.second->sensitive()->sensitiveDetector());
      output_t*        lc  = new lcio::LCCollectionVec(lcio::LCIO::SIMCALORIMETERHIT);
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
  }    // End namespace Simulation
}      // End namespace DD4hep



