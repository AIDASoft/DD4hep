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
#include <DD4hep/InstanceCount.h>
#include <DDDigi/DigiData.h>
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiEventAction.h>
#include <DDDigi/DigiParallelWorker.h>


/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Default base class for all Digitizer actions and derivates thereof.
    /**
     *  This is a utility class supporting properties, output and access to
     *  event and run objects through the context.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiIPRandomizer : public DigiEventAction   {
    protected:
      /// Property: Masks to act on
      int              m_mask   { 0 };
      /// Property: Input data segment name
      std::string      m_input  { };

      struct worker_args  {
	std::size_t num_hit  { 0 };
	std::size_t num_particle  { 0 };
	std::any*   container { nullptr };
	worker_args(std::any* p) : container(p) {}
      };
      struct work_t {
	DigiContext& context;
	Position     ip_move;
	std::vector<worker_args> args;
      };
      class work_definition_t;
      using self_t  = DigiIPRandomizer;
      using Worker  = DigiParallelWorker<self_t,work_t>;
      using Workers = DigiParallelWorkers<Worker>;

      /// Worker objects to be submitted to the kernel
      Workers m_workers;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiIPRandomizer);

      /// Default destructor
      virtual ~DigiIPRandomizer()  {
	InstanceCount::decrement(this);
      }

      Position ip_displacement()  const  {
	return Position(0,0,0);
      }

    public:
      /// Standard constructor
      DigiIPRandomizer(const DigiKernel& krnl, const std::string& nam)
	: DigiEventAction(krnl, nam)
      {
	declareProperty("mask", m_mask);
	declareProperty("input_segment", m_input = "inputs");
	InstanceCount::increment(this);
      }
      /// Main functional callback
      virtual void execute(DigiContext& context)  const  final  {
	auto& inputs = context.event->get_segment(m_input);
	std::vector<worker_args> collection_args;
	collection_args.reserve(inputs.size());
	for ( auto& i : inputs )     {
	  Key key(i.first);
	  if ( key.mask() == m_mask )   {
	    collection_args.emplace_back(worker_args(&i.second));
	  }
	}
	work_t data  { context, ip_displacement(), collection_args };
	for( auto& c : collection_args )   {
	  if ( DepositMapping* m = std::any_cast<DepositMapping>(c.container) )    {
	    for( auto& dep : *m )
	      dep.second.position += data.ip_move;
	    c.num_hit += m->size();
	  }
	  else if ( DepositVector* v = std::any_cast<DepositVector>(c.container) )    {
	    for( auto& dep : *v )
	      dep.second.position += data.ip_move;
	    c.num_hit += v->size();
	  }
	  else if ( ParticleMapping* parts = std::any_cast<ParticleMapping>(c.container) )    {
	    for( auto& p : *parts )   {
	      auto& part = p.second;
	      part.end_position   += data.ip_move;
	      part.start_position += data.ip_move;
	    }
	    c.num_particle += parts->size();
	  }
	}
	std::size_t num_hit = 0;
	std::size_t num_particle = 0;
	for( const auto& c : collection_args )   {
	  num_hit += c.num_hit;
	  num_particle += c.num_particle;
	}
	info("%s+++ Moved coordinates of %6ld hits ans %6ld particles", 
	     context.event->id(), num_hit, num_particle);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiIPRandomizer)
