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

/// Framework include files
#include <DD4hep/Primitives.h>
#include <DD4hep/Printout.h>
#include "DigiIO.h"

/// C/C++ include files
#include <limits>

// =========================================================================
//  EDM4HEP specific stuff
// =========================================================================
#ifdef DD4HEP_USE_EDM4HEP

/// edm4hep include files
#include <edm4hep/SimTrackerHit.h>
#include <edm4hep/MCParticle.h>
#include <edm4hep/MCParticleCollection.h>
#include <edm4hep/TrackerHitCollection.h>
#include <edm4hep/SimTrackerHitCollection.h>
#include <edm4hep/CalorimeterHitCollection.h>
#include <edm4hep/SimCalorimeterHitCollection.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    struct bla  {
      class my_part;
      typedef my_part particle_type;
    };

    edm4hep::Vector3d _toVectorD(const Position& ep);
    edm4hep::Vector3f _toVectorF(const Position& ep);

    /// Structure definitions for DDDigi input data
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct digi_input  {
      typedef Particle particle_type;
      struct  input_trackerhit_type     {};
      struct  input_calorimeterhit_type {};
    };

    /// Structure definitions for edm4hep input data
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct edm4hep_input  {
      typedef edm4hep::MutableMCParticle particle_type;
      struct  input_trackerhit_type     {};
      struct  input_calorimeterhit_type {};
    };

    /// Structure definitions for DDG4 input data
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    struct ddg4_input  {
      typedef sim::Geant4Particle particle_type;
      struct  input_trackerhit_type {};
      struct  input_calorimeterhit_type {};
    };

    template <typename T> struct data_input   {
      using particle_t       = typename T::particle_type;
      using trackerhit_t     = typename T::input_trackerhit_type;
      using calorimeterhit_t = typename T::input_calorimeterhit_type;
      using pwrap_t          = std::shared_ptr<particle_t>;
      using twrap_t          = std::shared_ptr<trackerhit_t>;
      using cwrap_t          = std::shared_ptr<calorimeterhit_t>;
    };

    edm4hep::Vector3d _toVectorD(const dd4hep::Position& ep)  {
      return { ep.x(), ep.y(), ep.z() };
    }

    edm4hep::Vector3f _toVectorF(const dd4hep::Position& ep)  {
      return { float(ep.x()), float(ep.y()), float(ep.z()) };
    }
    namespace {
      template <typename DATA> bool internal_can_handle(const DATA&, const std::type_info&)   {
        return true;
      }
      template <> bool internal_can_handle(const ParticleMapping::value_type& data, const std::type_info& info)   {
        return (data.second.source.type() == info);
      }
    }

    template <typename INPUT, typename DATA>
    static bool _can_handle(const INPUT& , const DATA& data)  {
      return internal_can_handle(data, typeid(typename data_input<INPUT>::pwrap_t));
    }

    template <typename CONT>
    void _pre_create(CONT* coll, std::size_t n)  {
      /// We have to pre-create all objects to be able to fill the parent-daughter relationships
      for ( std::size_t i=0; i<n; ++i )   {
        coll->create();
      }
    }

    template <typename INPUT, typename CONT>
    std::vector<const typename INPUT::particle_t*> _to_vector(const INPUT&, const CONT& cont)   {
      std::vector<const typename INPUT::particle_t*> vec;
      vec.reserve(cont.size());
      for ( const auto& part : cont )   {
        const auto& p = part.second;
        if ( p.source.type() == typeid(typename INPUT::pwrap_t) )   {
          const auto* ptr = std::any_cast<typename INPUT::pwrap_t>(&p.source);
          vec.emplace_back(ptr->get());
        }
      }
      if ( cont.size() != vec.size() )   {
        except("data_io","_to_vector: Containers of mixed origin are not supported!");
      }
      return vec;
    }

    template <typename T> template <typename FIRST, typename SECOND>
    void data_io<T>::_to_edm4hep(const FIRST&, SECOND)  {
      except("data_io::_to_edm4hep","(%s&, %s): Implementation not present!",
             typeName(typeid(FIRST)).c_str(), typeName(typeid(SECOND)).c_str());
    }

    /// Set all properties of the MutableMCParticle
    template <> template <>
    void data_io<digi_input>::_to_edm4hep(const Particle& p, 
                                          edm4hep::MutableMCParticle mcp)  {
      mcp.setPDG(p.pdgID);
      mcp.setTime(p.time);
      mcp.setMass(p.mass);
      mcp.setCharge(3.0*p.charge);
      mcp.setVertex( _toVectorD(p.start_position) );
      mcp.setEndpoint( _toVectorD(p.end_position) );
      mcp.setMomentum( _toVectorF(p.momentum) );
      mcp.setMomentumAtEndpoint( _toVectorF(p.momentum) );
    }

    template <> template <>
    void data_io<digi_input>::_to_edm4hep(const std::vector<const Particle*>& cont,
                                          edm4hep::MCParticleCollection* coll)   {
      std::size_t i, n = cont.size();
      _pre_create(coll, n);
      /// Convert particle body
      for ( i=0; i<n; ++i)   {
        _to_edm4hep(*cont[i], coll->at(i));
      }
    }

    /// Set all properties of the MutableMCParticle
    template <> template <>
    void data_io<edm4hep_input>::_to_edm4hep(const edm4hep::MCParticle& p, 
                                             edm4hep::MutableMCParticle mcp)
    {
      mcp.setPDG( p.getPDG() );
      mcp.setMomentum( p.getMomentum() );
      mcp.setMomentumAtEndpoint( p.getMomentumAtEndpoint() );
      mcp.setVertex(   p.getVertex() );
      mcp.setEndpoint( p.getEndpoint() );
      mcp.setTime( p.getTime() );
      mcp.setMass( p.getMass() );
      mcp.setCharge( p.getCharge() );
      mcp.setGeneratorStatus( p.getGeneratorStatus() );
      mcp.setSimulatorStatus( p.getSimulatorStatus() );
      mcp.setSpin(p.getSpin());
      mcp.setColorFlow(p.getColorFlow());
    }

    template <> template <>
    void data_io<edm4hep_input>::_to_edm4hep(const std::vector<const edm4hep::MCParticle*>& cont,
                                             edm4hep::MCParticleCollection* coll)
    {
      std::size_t i, n = cont.size();
      _pre_create(coll, n);
      /// Convert particle body
      for ( i=0; i<n; ++i)   {
        const auto* p = cont[i];
        auto mcp = coll->at(i);
        _to_edm4hep(*p, mcp);
#if 0
        /// Relationships are already resolved and kept in order: Just copy indices
        for (std::size_t idau = 0; idau < p->daughters_size(); ++idau)  {
          mcp.addToDaughters(coll->at(idau));
        }
        for (auto ipar : p->parents)   {
          mcp.addToParents(coll->at(ipar));
        }
#endif
      }
    }

    template <> template <> 
    void data_io<edm4hep_input>::_to_edm4hep(const std::pair<const CellID, EnergyDeposit>& dep,
					     const std::array<float, 6>& covMat,
					     edm4hep::TrackerHitCollection& collection,
					     int hit_type)

    {
      const EnergyDeposit& de = dep.second;
      auto hit = collection.create();
      double dep_error = de.depositError;
      if ( dep_error < -std::numeric_limits<double>::epsilon() )   {
        dep_error = 0e0;
      }
      hit.setType( hit_type );
      hit.setTime( de.time );
      hit.setCovMatrix( covMat );
      hit.setCellID( dep.first );
      hit.setEDep( de.deposit );
      hit.setEDepError( dep_error );
      //hit.setEdx( de.deposit/de.length );
      hit.setPosition( _toVectorD(de.position) );
    }

    template <> template <>
    void data_io<edm4hep_input>::_to_edm4hep(const std::pair<const CellID, EnergyDeposit>& dep,
					     edm4hep::CalorimeterHitCollection& collection,
					     int hit_type)
    {
      const EnergyDeposit& de = dep.second;
      auto hit = collection.create();
      double dep_error = de.depositError;
      if ( dep_error < -std::numeric_limits<double>::epsilon() )   {
        dep_error = 0e0;
      }
      hit.setType( hit_type );
      hit.setTime( de.time );
      hit.setCellID( dep.first );
      hit.setEnergy( de.deposit );
      hit.setEnergyError( dep_error );
      hit.setPosition( _toVectorF(de.position) );
    }
  }     // End namespace digi
}       // End namespace dd4hep
#endif  // DD4HEP_USE_EDM4HEP

/// =========================================================================
///  Conversion from DDG4 in memory to DDDigi
/// =========================================================================
#if defined(DD4HEP_USE_DDG4)

#include <DDG4/Geant4Data.h>
#include <DDG4/Geant4Particle.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    using PropertyMask = dd4hep::detail::ReferenceBitMask<int>;

    template <> template <>
    bool DepositPredicate<EnergyCut>::operator()(sim::Geant4Tracker::Hit* h)  const   {
      return h->energyDeposit > data.cutoff;
    }

    template <> template <>
    bool DepositPredicate<EnergyCut>::operator()(sim::Geant4Calorimeter::Hit* h)  const   {
      return h->energyDeposit > data.cutoff;
    }

    void add_particle_history(const sim::Geant4Calorimeter::Hit* hit, Key key, History& hist) {
      for( const auto& truth : hit->truth )   {
        key.set_item(truth.trackID);
        hist.particles.emplace_back(key, truth.deposit);
      }
    }

    void add_particle_history(const sim::Geant4Tracker::Hit* hit, Key key, History& hist)  {
      key.set_item(hit->truth.trackID);
      hist.particles.emplace_back(key, hit->truth.deposit);
    }

    template <> template <>
    void data_io<ddg4_input>::_to_digi_if(const std::vector<sim::Geant4Tracker::Hit*>& data,
                                          std::map<CellID, std::shared_ptr<sim::Geant4Tracker::Hit> >& hits,
                                          const DepositPredicate<EnergyCut>& predicate)   {
      for( auto* p : data )   {
        std::shared_ptr<sim::Geant4Tracker::Hit> ptr(p);
        if ( predicate(p) )   {
          CellID cell = ptr->cellID;
          hits.emplace(cell, std::move(ptr));
        }
      }
    }

    template <> template <>
    void data_io<ddg4_input>::_to_digi_if(const std::vector<sim::Geant4Calorimeter::Hit*>& data,
                                          std::map<CellID, std::shared_ptr<sim::Geant4Calorimeter::Hit> >& hits,
                                          const DepositPredicate<EnergyCut>& predicate)   {
      for( auto* p : data )   {
        std::shared_ptr<sim::Geant4Calorimeter::Hit> ptr(p);
        if ( predicate(p) )   {
          CellID cell = ptr->cellID;
          hits.emplace(cell, std::move(ptr));
        }
      }
    }

    template <> template <>
    void data_io<ddg4_input>::_to_digi(Key key, 
                                       const std::vector<sim::Geant4Particle*>& input,
                                       ParticleMapping& particles)
    {
      Key mkey = key;
      for( auto* part_ptr : input )   {
        std::shared_ptr<sim::Geant4Particle> p(part_ptr);
        Particle part;
        part.start_position = Position(p->vsx, p->vsy, p->vsz);
        part.end_position   = Position(p->vex, p->vey, p->vez);
        part.momentum       = Direction(p->psx,p->psy, p->psz);
        part.pdgID          = p->pdgID;
        part.charge         = p->charge;
        part.mass           = p->mass;
        part.time           = p->time;
        mkey.set_item(particles.size());
        part.source = std::make_any<std::shared_ptr<sim::Geant4Particle> >(std::move(p));
        particles.push(mkey, std::move(part));
      }
    }

    template <typename T>
    static void cnv_to_digi(Key key,
                            const std::pair<const CellID, std::shared_ptr<T> >& depo,
                            DepositVector& out)     {
      Key history_key;
      EnergyDeposit dep { };
      const auto* h = depo.second.get();
      Position pos = h->position;
      pos *= 1./dd4hep::mm;

      dep.flag = h->flag;
      dep.deposit = h->energyDeposit;
      dep.position = pos;

      history_key.set_mask(key.mask());
      history_key.set_item(out.size());
      history_key.set_segment(key.segment());
      dep.history.hits.emplace_back(history_key, dep.deposit);
      add_particle_history(h, history_key, dep.history);
      out.emplace(depo.first, std::move(dep));
    }

    template <> template <>
    void data_io<ddg4_input>::_to_digi(Key key,
                                       const std::map<CellID, std::shared_ptr<sim::Geant4Calorimeter::Hit> >& hits,
                                       DepositVector& out)  {
      out.data_type = SegmentEntry::CALORIMETER_HITS;
      for( const auto& p : hits )
        cnv_to_digi(key, p, out);
    }

    template <> template <>
    void data_io<ddg4_input>::_to_digi(Key key, 
                                       const std::map<CellID, std::shared_ptr<sim::Geant4Tracker::Hit> >& hits,
                                       DepositVector& out)  {
      out.data_type = SegmentEntry::TRACKER_HITS;
      for( const auto& p : hits )
        cnv_to_digi(key, p, out);
    }
  }     // End namespace digi
}       // End namespace dd4hep
#endif  // DD4HEP_USE_DDG4

/// ======================================================================
///  Conversion from DDG4 in memory to edm4hep
/// ======================================================================
#if defined(DD4HEP_USE_DDG4) && defined(DD4HEP_USE_EDM4HEP)

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Set all properties of the MutableMCParticle
    template <> template <>
    void data_io<ddg4_input>::_to_edm4hep(const sim::Geant4Particle& p, 
                                          edm4hep::MutableMCParticle mcp)
    {
      auto status = p.status;
      const PropertyMask mask(status);
      mcp.setPDG(p.pdgID);

      mcp.setMomentum( _toVectorF( { p.psx, p.psy, p.psz } ) );
      mcp.setMomentumAtEndpoint( _toVectorF( {p.pex, p.pey, p.pez} ) );
      mcp.setVertex( _toVectorD( { p.vsx, p.vsy, p.vsz } ) );
      mcp.setEndpoint( _toVectorD( { p.vex, p.vey, p.vez } ) );

      mcp.setTime(p.time);
      mcp.setMass(p.mass);
      mcp.setCharge(3.0*float(p.charge));

      // Set generator status
      mcp.setGeneratorStatus(0);
      if( p.genStatus ) {
        mcp.setGeneratorStatus( p.genStatus ) ;
      } else {
        if ( mask.isSet(sim::G4PARTICLE_GEN_STABLE) )             mcp.setGeneratorStatus(1);
        else if ( mask.isSet(sim::G4PARTICLE_GEN_DECAYED) )       mcp.setGeneratorStatus(2);
        else if ( mask.isSet(sim::G4PARTICLE_GEN_DOCUMENTATION) ) mcp.setGeneratorStatus(3);
        else if ( mask.isSet(sim::G4PARTICLE_GEN_BEAM) )          mcp.setGeneratorStatus(4);
        else if ( mask.isSet(sim::G4PARTICLE_GEN_OTHER) )         mcp.setGeneratorStatus(9);
      }

      // Set simulation status
      mcp.setCreatedInSimulation(         mask.isSet(sim::G4PARTICLE_SIM_CREATED) );
      mcp.setBackscatter(                 mask.isSet(sim::G4PARTICLE_SIM_BACKSCATTER) );
      mcp.setVertexIsNotEndpointOfParent( mask.isSet(sim::G4PARTICLE_SIM_PARENT_RADIATED) );
      mcp.setDecayedInTracker(            mask.isSet(sim::G4PARTICLE_SIM_DECAY_TRACKER) );
      mcp.setDecayedInCalorimeter(        mask.isSet(sim::G4PARTICLE_SIM_DECAY_CALO) );
      mcp.setHasLeftDetector(             mask.isSet(sim::G4PARTICLE_SIM_LEFT_DETECTOR) );
      mcp.setStopped(                     mask.isSet(sim::G4PARTICLE_SIM_STOPPED) );
      mcp.setOverlay(                     false );

      //fg: if simstatus !=0 we have to set the generator status to 0:
      if( mcp.isCreatedInSimulation() )
        mcp.setGeneratorStatus( 0 );

      mcp.setSpin(p.spin);
      mcp.setColorFlow(p.colorFlow);
    }

    template <> template <> 
    void data_io<ddg4_input>::_to_edm4hep(const std::vector<const sim::Geant4Particle*>& cont,
                                          edm4hep::MCParticleCollection* coll)
    {
      /// Precreate objects to allow setting the references without second pass
      std::size_t i, n = cont.size();
      _pre_create(coll, n);
      /// Convert particle body
      for ( i=0; i<n; ++i)   {
        const auto* p = cont[i];
        auto  mcp = coll->at(i);
        _to_edm4hep(*p, mcp);
        /// Relationships are already resolved and kept in order: Just copy indices
        for (auto idau : p->daughters)
          mcp.addToDaughters(coll->at(idau));
        for (auto ipar : p->parents)
          mcp.addToParents(coll->at(ipar));
      }
    }
  }     // End namespace digi
}       // End namespace dd4hep
#endif  // DD4HEP_USE_DDG4 && DD4HEP_USE_EDM4HEP

/// ======================================================================
///  Conversion from DDDigi in memory to edm4hep
/// ======================================================================
#ifdef DD4HEP_USE_EDM4HEP
/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    template <> template <> 
    void data_io<edm4hep_input>::_to_edm4hep(const ParticleMapping& cont,
					     edm4hep::MCParticleCollection* coll)
    {
      if ( cont.empty() )   {
        return;
      }
      else if ( _can_handle(edm4hep_input(), *cont.begin()) )  {
        auto vec = _to_vector(data_input<edm4hep_input>(), cont);
        if ( !vec.empty() )  {
	  data_io<edm4hep_input>::_to_edm4hep(vec, coll);
        }
      }
      else if ( _can_handle(ddg4_input(), *cont.begin()) )  {
        auto vec = _to_vector(data_input<ddg4_input>(), cont);
        if ( !vec.empty() )  {
          data_io<ddg4_input>::_to_edm4hep(vec, coll);
        }
      }
      else   {
	// Catch-all: convert what we have at hands
	auto vec = _to_vector(data_input<digi_input>(), cont);
	if ( !vec.empty() )  {
	  data_io<digi_input>::_to_edm4hep(vec, coll);
	}
      }
    }
  }     // End namespace digi
}       // End namespace dd4hep
#endif  // DD4HEP_USE_EDM4HEP
