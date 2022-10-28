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
    class DigiSegmentDepositExtractor : public DigiSegmentProcessor   {
    public:
      /// Standard constructor
      DigiSegmentDepositExtractor(const DigiKernel& kernel, const std::string& nam)
	: DigiSegmentProcessor(kernel, nam) {}

      template <typename T> void copy_deposits(const T& cont, work_t& work)  const  {
	DepositVector deposits(cont.name, work.output.mask);
	for( const auto& dep : cont )   {
	  CellID        cell = dep.first;
	  EnergyDeposit depo = dep.second;
	  deposits.data.emplace_back(cell, std::move(depo));
	}
        work.output.data.put(deposits.key, std::move(deposits));
      }
      /// Main functional callback
      virtual void execute(DigiContext&, work_t& work)  const override final  {
	if ( segment.matches(work.input.key.value()) )   {
	  if ( const auto* m = work.get_input<DepositMapping>() )
	    copy_deposits(*m, work);
	  else if ( const auto* v = work.get_input<DepositVector>() )
	    copy_deposits(*v, work);
	  else
	    except("Request to handle unknown data type: %s", work.input_type_name().c_str());
	}
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSegmentDepositExtractor)
