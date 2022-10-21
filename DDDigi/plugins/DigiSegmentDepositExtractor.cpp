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

      /// Main functional callback
      virtual void handle_segment(DigiContext&, work_t& work)  const override final  {
	for( const auto& d : work.input )   {
	  if ( segment.matches(d.first) )   {
	    CellID        cell = d.first;
	    EnergyDeposit depo = d.second;
	    work.emplace_output(cell, std::move(depo));
	  }
	}
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSegmentDepositExtractor)
