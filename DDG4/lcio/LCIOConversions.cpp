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

    template <> lcio::LCCollectionVec*
    Geant4DataConversion<lcio::LCCollectionVec,pair<VolMgr,G4VHitsCollection*>,Geant4HitCollection>::
    operator()(const arg_t& args)  const {
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

    template <> lcio::LCCollectionVec* 
    Geant4DataConversion<lcio::LCCollectionVec,pair<VolMgr,Geant4HitCollection*>,SimpleTracker::Hit>::operator()(const arg_t& args)  const 
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

    template <> lcio::LCCollectionVec* 
    Geant4DataConversion<lcio::LCCollectionVec,pair<VolMgr,Geant4HitCollection*>,SimpleCalorimeter::Hit>::operator()(const arg_t& args)  const 
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

    typedef pair<VolMgr,G4VHitsCollection*> _AA1;
    template class Geant4Conversion<lcio::LCCollectionVec,_AA1>;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA1,Geant4HitCollection)
    typedef pair<VolMgr,Geant4HitCollection*> _AA2;
    template class Geant4Conversion<lcio::LCCollectionVec,_AA2>;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA2,SimpleTracker::Hit)
    typedef pair<VolMgr,Geant4HitCollection*> _AA3;
    DECLARE_GEANT4_HITCONVERTER(lcio::LCCollectionVec,_AA3,SimpleCalorimeter::Hit)
  }    // End namespace Simulation
}      // End namespace DD4hep



