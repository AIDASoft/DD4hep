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
#include "DDG4/Geant4HitCollection.h"
#include "DDG4/Geant4DataConversion.h"
#include "DDG4/Geant4Data.h"

// LCIO includes
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
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
			 SimpleTracker::Hit>::operator()(const arg_t& args)  const 
    {
      Geant4HitCollection* coll = args.second;
      size_t     nhits = coll->GetSize();
      SimpleHit* hit   = coll->hit(0);
      string     dsc   = encoding(args.first, hit->cellID);
      lcio::LCCollectionVec* col = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      UTIL::CellIDEncoder<SimTrackerHit> decoder(dsc,col);  
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
	col->addElement(lc_hit);
      }
      return col;
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
			 SimpleCalorimeter::Hit>::operator()(const arg_t& args)  const 
    {
      Geant4HitCollection* coll = args.second;
      size_t     nhits = coll->GetSize();
      SimpleHit* hit   = coll->hit(0);
      string     dsc   = encoding(args.first, hit->cellID);
      lcio::LCCollectionVec* col = new lcio::LCCollectionVec(lcio::LCIO::SIMCALORIMETERHIT);	
      UTIL::CellIDEncoder<SimCalorimeterHit> decoder(dsc,col);  
      col->setFlag(UTIL::make_bitset32(LCIO::CHBIT_LONG,LCIO::CHBIT_STEP)); 
      for(size_t i=0; i<nhits; ++i)   {
	const SimpleCalorimeter::Hit* g4_hit = coll->hit(i);
	float pos[3] = {g4_hit->position.x(), g4_hit->position.y(), g4_hit->position.z()};
	lcio::SimCalorimeterHitImpl*  lc_hit = new lcio::SimCalorimeterHitImpl;
	lc_hit->setCellID0( ( g4_hit->cellID >>    0          ) & 0xFFFFFFFF ); 
	lc_hit->setCellID1( ( g4_hit->cellID >> sizeof( int ) ) & 0xFFFFFFFF );
	lc_hit->setPosition(pos);
	lc_hit->setEnergy( g4_hit->energyDeposit );
	col->addElement(lc_hit);
      }
      return col;
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
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      for(size_t i=0, nhits = coll->GetSize(); i<nhits; ++i)   {
	Geant4HitWrapper& wrap = coll->hit(i);
	lcio::SimTrackerHitImpl* lc_hit = wrap;
	wrap.release();  // Now we have ownership!
	lc_coll->addElement(lc_hit);
      }
      coll->clear();
      return lc_coll;
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
      lcio::LCCollectionVec* lc_coll = new lcio::LCCollectionVec(lcio::LCIO::SIMTRACKERHIT);
      for(size_t i=0, nhits = coll->GetSize(); i<nhits; ++i)   {
	Geant4HitWrapper& wrap = coll->hit(i);
	lcio::SimCalorimeterHitImpl* lc_hit = wrap;
	wrap.release();  // Now we have ownership!
	lc_coll->addElement(lc_hit);
      }
      coll->clear();
      return lc_coll;
    }

    typedef pair<VolMgr,G4VHitsCollection*> _AA1;
    typedef pair<VolMgr,Geant4HitCollection*> _AA2;
    template class Geant4Conversion<lcio::LCCollectionVec,_AA1>;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA1,Geant4HitCollection)

    template class Geant4Conversion<lcio::LCCollectionVec,_AA2>;
    // Hit converters for simple Geant4Data objects
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA2,SimpleTracker::Hit)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA2,SimpleCalorimeter::Hit)
    // Hit converters for standard LCIO objects
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA2,lcio::SimTrackerHitImpl)
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA2,lcio::SimCalorimeterHitImpl)
  }    // End namespace Simulation
}      // End namespace DD4hep



