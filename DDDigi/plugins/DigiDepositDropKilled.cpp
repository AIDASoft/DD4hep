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
#include <DDDigi/DigiContainerProcessor.h>
#include <DD4hep/InstanceCount.h>
#include <DD4hep/DD4hepUnits.h>

/// C/C++ include files
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to drop energy deposits of a container having the "KILLED" flag
    /**
     *
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiDepositDropKilled : public DigiContainerProcessor   {
    protected:
      /// Property: Flag to update existing container in-place or create a new container
      bool   m_update_in_place            { true  };

    public:
      /// Standard constructor
      DigiDepositDropKilled(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("update_in_place",            m_update_in_place = true);
	InstanceCount::increment(this);
      }

      /// Default destructor
      virtual ~DigiDepositDropKilled() {
	InstanceCount::decrement(this);
      }
      template <typename T> std::size_t length(const T* m)  const {  return m->size(); }
      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t&)  const override final  {
	std::size_t killed = 0, total = 0, i = 0;
	if ( auto* v = work.get_input<DepositVector>() )   {
	  total = length(v);
	  for( auto iter = v->begin(); iter != v->end(); ++iter, ++i )   {
	    if ( v->at(i).flag&EnergyDeposit::KILLED )   {
	      v->remove(iter);
	      iter = v->begin() + (--i);
	    }
	  }
	  killed = total - length(v);
	}
	else if ( auto* m = work.get_input<DepositMapping>() )   {
	  CellID last_cell = ~0x0LL;
	  total = length(m);
	  for( auto iter = m->begin(); iter != m->end(); ++iter )   {
	    if ( iter->second.flag&EnergyDeposit::KILLED )   {
	      m->remove(iter);
	      iter = (last_cell != ~0x0LL) ? m->data.find(last_cell) : m->begin();
	      continue;
	    }
	    last_cell = iter->first;
	  }
	  killed = total - length(v);
	}
	else   {
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
	}
	info("%s+++ Killed %6ld out of %6ld deposit entries from container: %s",context.event->id(), killed, total);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiDepositDropKilled)
