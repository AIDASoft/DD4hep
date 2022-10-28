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
#include <DDDigi/DigiContext.h>
#include <DDDigi/DigiSegmentProcessor.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Helper class to print energy deposits pre-selected by the segment splitter
    /** Helper class to print energy deposits pre-selected by the segment splitter
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    class DigiSegmentDepositPrint : public DigiSegmentProcessor   {
    public:
      /// Standard constructor
      DigiSegmentDepositPrint(const DigiKernel& kernel, const std::string& nam)
	: DigiSegmentProcessor(kernel, nam) {}

      void print_deposit(const char* format, CellID cell, const EnergyDeposit& depo)  const   {
	info(format, segment.split_id(cell), cell,
	     depo.history.hits.size(), 
	     depo.history.particles.size(),
	     depo.deposit);
      }
      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work)  const override final  {
	char format[256];
	::snprintf(format, sizeof(format), 
		   "%s[%s] %s-id: %%d [processor:%d] Cell: %%016lX mask: %016lX  hist:%%4ld hits %%4ld parts. entries deposit: %%f", 
		   context.event->id(), segment.idspec.name(), segment.cname(), segment.id, segment.split_mask);
	auto call = [this, format](const std::pair<CellID,EnergyDeposit>& d)   {
	  if( this->segment.matches(d.first) )
	    this->print_deposit(format, d.first, d.second); 
	};
	if ( const auto* m = work.get_input<DepositMapping>() )
	  std::for_each(m->begin(), m->end(), call);
	else if ( const auto* v = work.get_input<DepositVector>() )
	  std::for_each(v->begin(), v->end(), call);
	else
	  error("+++ Request to dump an invalid container %s", Key::key_name(work.input.key).c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSegmentDepositPrint)
