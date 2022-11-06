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
    class DigiHitHistoryDrop : public DigiEventAction   {
    protected:
      /// Property: Masks to act on
      std::vector<int> m_masks  { };
      /// Property: Input data segment name
      std::string      m_input  { };

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiHitHistoryDrop);

      /// Default destructor
      virtual ~DigiHitHistoryDrop()  {
	InstanceCount::decrement(this);
      }

    public:
      /// Standard constructor
      DigiHitHistoryDrop(const DigiKernel& krnl, const std::string& nam)
	: DigiEventAction(krnl, nam)
      {
	declareProperty("masks", m_masks);
	declareProperty("input", m_input = "inputs");
	InstanceCount::increment(this);
      }

      template <typename T>
      std::pair<std::size_t, std::size_t> drop_history(T& cont)  const  {
	std::size_t num_drop_hit = 0;
	std::size_t num_drop_particle = 0;
	for( auto& dep : cont )    {
	  auto ret = dep.second.history.drop();
	  num_drop_hit += ret.first;
	  num_drop_particle += ret.second;
	}
	return std::make_pair(num_drop_hit,num_drop_particle);
      }

      /// Main functional callback
      virtual void execute(DigiContext& context)  const  final  {
	auto& inputs = context.event->get_segment(m_input);
	std::size_t num_drop_hit = 0;
	std::size_t num_drop_particle = 0;
	for( auto& i : inputs )     {
	  Key key(i.first);
	  auto im = std::find(m_masks.begin(), m_masks.end(), key.mask());
	  if( im != m_masks.end() )   {
	    if( DepositMapping* m = std::any_cast<DepositMapping>(&i.second) )    {
	      auto ret = drop_history(*m);
	      num_drop_hit += ret.first;
	      num_drop_particle += ret.second;
	    }
	    if( DepositVector* m = std::any_cast<DepositVector>(&i.second) )    {
	      auto ret = drop_history(*m);
	      num_drop_hit += ret.first;
	      num_drop_particle += ret.second;
	    }
	  }
	}
	info("%s+++ Dropped history of %6ld hits %6ld particles", 
	     context.event->id(), num_drop_hit, num_drop_particle);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiHitHistoryDrop)
