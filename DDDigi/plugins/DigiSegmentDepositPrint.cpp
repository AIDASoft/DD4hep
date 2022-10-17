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
#include <DDDigi/DigiSegmentationSplitter.h>

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
    class DigiSegmentDepositPrint : public DigiSegmentAction   {
    public:
      /// Constructors used of base class
      using DigiSegmentAction::DigiSegmentAction;

      /// Main functional callback
      virtual DepositVector 
      handleSegment(DigiContext&              context,
		    const DigiSegmentContext& segment,
		    const DepositMapping&     deposits)  const override final   {
	char format[256];
	::snprintf(format, sizeof(format), 
		   "%s[%s] %s-id: %%d [processor:%d] Cell: %%016lX mask: %016lX  hist:%%4ld entries deposit: %%f", 
		   context.event->id(), segment.idspec.name(), segment.cname(), segment.id, segment.split_mask);
	for( const auto& d : deposits )   {
	  if ( segment.matches(d.first) )   {
	    auto  cell = d.first;
	    auto& depo = d.second;
	    info(format, segment.split_id(cell), cell, depo.history.size(), depo.deposit);
	  }
	}
	return {};
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGISEGMENTACTION_NS(dd4hep::digi,DigiSegmentDepositPrint)
