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
#include <DDDigi/DigiSegmentProcessor.h>

/// C/C++ include files
#include <cmath>
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {
  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiSimpleADCResponse : public DigiSegmentProcessor  {
      std::string m_name_postfix       { ".adc" };
      double      m_use_segmentation   { false  };
      double      m_signal_cutoff      { std::numeric_limits<double>::epsilon() };
      double      m_signal_saturation  { std::numeric_limits<double>::max() };
      std::size_t m_adc_resolution     { 1024 };

    public:
      /// Standard constructor
      DigiSimpleADCResponse(const DigiKernel& krnl, const std::string& nam)
	: DigiSegmentProcessor(krnl, nam)
      {
	declareProperty("cutoff",         m_signal_cutoff);
	declareProperty("saturation",     m_signal_saturation);
	declareProperty("adc_resolution", m_adc_resolution);
	declareProperty("name_postfix",   m_name_postfix);
      }

      /// Create container with ADC counts and register it to the output segment
      template <typename T, typename P>
      void create_adc_counts(const char* tag, const T& input, work_t& work, const P& predicate)  const  {
	std::string postfix = m_use_segmentation ? "."+segment.identifier() : std::string();
	std::string outname = input.name + postfix + m_name_postfix;
	DetectorResponse response(outname, work.output.mask);
	for( const auto& dep : input )   {
	  if ( predicate(dep) )   {
	    CellID      cell = dep.first;
	    const auto& depo = dep.second;
	    ADCValue::value_t adc_count = std::round((depo.deposit * m_adc_resolution) / m_signal_saturation);
	    response.emplace(cell, {adc_count, ADCValue::address_t(cell)});
	  }
	}
	info("%s+++ %-32s %6ld ADC values. Input: %-32s %6ld deposits", tag,
	     outname.c_str(), response.size(), input.name.c_str(), input.size());
        work.output.data.put(response.key, std::move(response));
      }
      /// Create container with ADC counts and register it to the output segment
      template <typename P>
      void create_adc_counts(DigiContext& context, work_t& work, const P& predicate)  const  {
	const char* tag = context.event->id();
	if ( const auto* m = work.get_input<DepositMapping>() )
	  create_adc_counts(tag, *m, work, predicate);
	else if ( const auto* v = work.get_input<DepositVector>() )
	  create_adc_counts(tag, *v, work, predicate);
	else
	  except("%s+++ Request to handle unknown data type: %s",
		 tag, work.input_type_name().c_str());
      }
      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work)  const override final  {
	if ( !m_use_segmentation )
	  create_adc_counts(context, work, accept_all_t());
	else if ( segment.matches(work.input.key.key) )
	  create_adc_counts(context, work, accept_segment_t(segment));
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep

#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSimpleADCResponse)
