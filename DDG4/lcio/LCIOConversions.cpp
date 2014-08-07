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
#include "DDG4/Geant4Data.h"

// LCIO includes
#include "IMPL/LCCollectionVec.h"
//
#include "IMPL/ClusterImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/MCParticleImpl.h"
//
#include "UTIL/Operators.h"
#include "UTIL/ILDConf.h"
using namespace std;

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /*
   *   Simulation namespace declaration
   */
  namespace Simulation   {

    typedef Geometry::VolumeManager VolMgr;

    /// Data conversion interface calling lower level explicit convetrers
    /**
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,
			 pair<VolMgr,G4VHitsCollection*>,
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

    /// Data conversion interface creating lcio::SimTrackerHitImpl from SimpleTracker::Hit structures
    /**
     *  This converter is to be used, when the sensitive detectors create fill collections
     *  of type Geant4HitCollection with objects of type **SimpleTracker::Hit**.
     *  The original objects are untouched and are automatically when the hosting
     *  Geant4HitCollection object is released.
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec* 
    Geant4DataConversion<lcio::LCCollectionVec,
			 pair<VolMgr,Geant4HitCollection*>,
			 SimpleTracker::Hit>::operator()(const arg_t& args)  const   {

      Geant4HitCollection* coll = args.second;
      size_t     nhits = coll->GetSize();
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      lc_coll->reserve(nhits);
      if ( nhits > 0 )  {
	SimpleHit* hit   = coll->hit(0);
	string     dsc   = encoding(args.first, hit->cellID);
	UTIL::CellIDEncoder<SimTrackerHit> decoder(dsc,lc_coll);  
	for(size_t i=0; i<nhits; ++i)   {
	  const SimpleTracker::Hit* g4_hit = coll->hit(i);
	  double pos[3] = {g4_hit->position.x(), g4_hit->position.y(), g4_hit->position.z()};
	  lcio::SimTrackerHitImpl* lc_hit = new lcio::SimTrackerHitImpl;  
	  lc_hit->setCellID0( (g4_hit->cellID >>    0          ) & 0xFFFFFFFF); 
	  lc_hit->setCellID1( (g4_hit->cellID >> sizeof( int ) ) & 0xFFFFFFFF);
	  lc_hit->setPosition(pos);
	  lc_hit->setEDep(g4_hit->energyDeposit);
	  lc_hit->setTime(g4_hit->truth.time);
	  lc_hit->setMomentum( g4_hit->momentum.x(), g4_hit->momentum.y() , g4_hit->momentum.z() );
	  lc_hit->setPathLength( g4_hit->length);
	  lc_coll->addElement(lc_hit);
	}
      }
      return lc_coll;
    }

    /// Data conversion interface creating lcio::SimCalorimeterHitImpl from SimpleCalorimeter::Hit structures
    /**
     *  This converter is to be used, when the sensitive detectors create fill collections
     *  of type Geant4HitCollection with objects of type **SimpleCalorimeter::Hit**.
     *  The original objects are untouched and are automatically when the hosting
     *  Geant4HitCollection object is released.
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec* 
    Geant4DataConversion<lcio::LCCollectionVec,
			 pair<VolMgr,Geant4HitCollection*>,
			 SimpleCalorimeter::Hit>::operator()(const arg_t& args)  const  {
      Geant4HitCollection* coll = args.second;
      size_t     nhits = coll->GetSize();
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::SIMCALORIMETERHIT);	
      lc_coll->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_STEP,LCIO::CHBIT_ID1)); 
      lc_coll->reserve(nhits);
      if ( nhits > 0 )   {
	SimpleHit* hit   = coll->hit(0);
	string     dsc   = encoding(args.first, hit->cellID);
	UTIL::CellIDEncoder<SimCalorimeterHit> decoder(dsc,lc_coll);
	for(size_t i=0; i<nhits; ++i)   {
	  const SimpleCalorimeter::Hit* g4_hit = coll->hit(i);
	  float pos[3] = {g4_hit->position.x(), g4_hit->position.y(), g4_hit->position.z()};
	  lcio::SimCalorimeterHitImpl*  lc_hit = new lcio::SimCalorimeterHitImpl;
	  lc_hit->setCellID0( ( g4_hit->cellID >>    0          ) & 0xFFFFFFFF ); 
	  lc_hit->setCellID1( ( g4_hit->cellID >> sizeof( int ) ) & 0xFFFFFFFF );
	  lc_hit->setPosition(pos);
	  lc_hit->setEnergy( g4_hit->energyDeposit );
	  lc_coll->addElement(lc_hit);
	}
      }
      return lc_coll;
    }

    template <typename T> lcio::LCCollectionVec* moveEntries(Geant4HitCollection* coll, lcio::LCCollectionVec* lc_coll)  {
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
			 pair<VolMgr,Geant4HitCollection*>,
			 lcio::SimTrackerHitImpl>::operator()(const arg_t& args)  const 
    {
      Geant4HitCollection* coll = args.second;
      lcio::SimTrackerHitImpl* hit = coll->hit(0);
      long long int id1 = hit->getCellID1(), id0=hit->getCellID0();
      long long int cellID = (((id1<<32)&0xFFFFFFFF00000000)|(id0&0xFFFFFFFF));
      string dsc = encoding(args.first, cellID);
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      UTIL::CellIDEncoder<SimTrackerHit> decoder(dsc,lc_coll);  
      return moveEntries<lcio::SimTrackerHitImpl>(args.second,lc_coll);
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
			 pair<VolMgr,Geant4HitCollection*>,
			 lcio::SimCalorimeterHitImpl>::operator()(const arg_t& args)  const 
    {
      Geant4HitCollection* coll = args.second;
      lcio::SimCalorimeterHitImpl* hit = coll->hit(0);
      output_t* lc = new lcio::LCCollectionVec(lcio::LCIO::SIMCALORIMETERHIT);
      long long int id1 = hit->getCellID1(), id0=hit->getCellID0();
      long long int cellID = (((id1<<32)&0xFFFFFFFF00000000)|(id0&0xFFFFFFFF));
      string dsc = encoding(args.first, cellID);
      UTIL::CellIDEncoder<SimCalorimeterHit> decoder(dsc,lc);
      lc->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_STEP,LCIO::CHBIT_ID1)); 
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
			 pair<VolMgr,Geant4HitCollection*>,
			 lcio::ClusterImpl>::operator()(const arg_t& args)  const 
    {
      output_t* lc = new lcio::LCCollectionVec(lcio::LCIO::CLUSTER);
      return moveEntries<tag_t>(args.second,lc);
    }
    /// Data conversion interface moving lcio::MCParticleImpl objects from a Geant4HitCollection to a LCCollectionVec
    /** Same comments apply as for the data mover for lcio::SimCalorimeterHitImpl and lcio::SimTrackerHitImpl
     *
     *  @author M.Frank
     *  @version 1.0
     */
    template <> lcio::LCCollectionVec* 
    Geant4DataConversion<lcio::LCCollectionVec,
			 pair<VolMgr,Geant4HitCollection*>,
			 lcio::MCParticleImpl>::operator()(const arg_t& args)  const 
    {
      output_t* lc = new lcio::LCCollectionVec(lcio::LCIO::MCPARTICLE);
      return moveEntries<tag_t>(args.second,lc);
    }

    typedef pair<VolMgr,G4VHitsCollection*> RAW_CONVERSION_ARGS;
    typedef pair<VolMgr,Geant4HitCollection*> CONVERSION_ARGS;
    template class Geant4Conversion<lcio::LCCollectionVec,RAW_CONVERSION_ARGS>;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,RAW_CONVERSION_ARGS,Geant4HitCollection)

    template class Geant4Conversion<lcio::LCCollectionVec,CONVERSION_ARGS>;
    // Hit converters for simple Geant4Data objects
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,SimpleTracker::Hit)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,SimpleCalorimeter::Hit)
    // Hit converters for standard LCIO objects
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::SimTrackerHitImpl)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::SimCalorimeterHitImpl)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::ClusterImpl)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,CONVERSION_ARGS,lcio::MCParticleImpl)
  }    // End namespace Simulation
}      // End namespace DD4hep



