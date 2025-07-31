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
    class DigiSegmentDepositExtractor : public DigiContainerProcessor   {
    public:
      /// Standard constructor
      DigiSegmentDepositExtractor(const DigiKernel& kernel, const std::string& nam)
        : DigiContainerProcessor(kernel, nam) {}

      template <typename T> void copy_deposits(const T& cont, work_t& work, const predicate_t& predicate)  const  {
	DepositVector deposits(cont.name, work.environ.output.mask, cont.data_type);
	for( const auto& dep : cont )   {
	  if( predicate(dep) )   {
	    CellID        cell = dep.first;
	    EnergyDeposit depo = dep.second;
	    deposits.emplace(cell, std::move(depo));
	  }
	}
        work.environ.output.data.put(deposits.key, std::move(deposits));
      }
      /// Main functional callback
      virtual void execute(DigiContext&, work_t& work, const predicate_t& predicate)  const override final  {
        if ( const auto* m = work.get_input<DepositMapping>() )
          copy_deposits(*m, work, predicate);
        else if ( const auto* v = work.get_input<DepositVector>() )
          copy_deposits(*v, work, predicate);
        else
          except("Request to handle unknown data type: %s", work.input_type_name().c_str());
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep
//        Factory definition
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSegmentDepositExtractor)
