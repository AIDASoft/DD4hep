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
#include <DDDigi/DigiContainerProcessor.h>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Actor to select energy deposits according to the supplied segmentation
    /** Actor to select energy deposits according to the supplied segmentation
     *
     *  The selected deposits are placed in the output container
     *  supplied by the arguments.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiCellMultiplicityCounter : public DigiContainerProcessor   {
    public:
      /// Standard constructor
      using DigiContainerProcessor::DigiContainerProcessor;

      template <typename T> void count_deposits(const char* tag, const T& cont)  const  {
	std::map<CellID, std::size_t> entries;
	for( const auto& dep : cont )   {
	  CellID        cell = dep.first;
	  entries[cell] += 1;
	}
	info("%s+++ %-32s has %6ld entries and %6ld unique cells",
	     tag, cont.name.c_str(), cont.size(), entries.size());
      }
      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t&)  const override final  {
	if ( const auto* m = work.get_input<DepositMapping>() )
	  count_deposits(context.event->id(), *m);
	else if ( const auto* v = work.get_input<DepositVector>() )
	  count_deposits(context.event->id(), *v);
	else
	  except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiCellMultiplicityCounter)
