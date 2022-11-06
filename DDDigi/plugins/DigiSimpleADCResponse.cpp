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
#include <DDDigi/DigiSegmentSplitter.h>

/// C/C++ include files
#include <cmath>
#include <limits>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    class DigiSimpleADCResponse : public DigiContainerProcessor  {
      using segmentation_t = DigiSegmentProcessContext;
      std::string    m_response_postfix   { ".adc" };
      std::string    m_history_postfix    { ".hist" };
      double         m_signal_cutoff      { std::numeric_limits<double>::epsilon() };
      double         m_signal_saturation  { std::numeric_limits<double>::max() };
      double         m_adc_offset         { 0e0  };
      std::size_t    m_adc_resolution     { 1024 };
      
      public:
      /// Standard constructor
      DigiSimpleADCResponse(const DigiKernel& krnl, const std::string& nam)
	: DigiContainerProcessor(krnl, nam)
      {
	declareProperty("cutoff",           m_signal_cutoff);
	declareProperty("saturation",       m_signal_saturation);
	declareProperty("adc_resolution",   m_adc_resolution);
	declareProperty("response_postfix", m_response_postfix);
	declareProperty("history_postfix",  m_history_postfix);
      }

      /// Create container with ADC counts and register it to the output segment
      template <typename T, typename P>
      void emulate_adc(const char* tag, const T& input, work_t& work, const P& predicate)  const  {
	std::string postfix = predicate.segmentation ? "."+predicate.segmentation->identifier(predicate.id) : std::string();
	std::string history_name  = input.name + postfix + m_history_postfix;
	std::string response_name = input.name + postfix + m_response_postfix;
	DetectorHistory  history (history_name, work.output.mask);
	DetectorResponse response(response_name, work.output.mask);
	for( const auto& dep : input )   {
	  if ( predicate(dep) )   {
	    CellID      cell = dep.first;
	    const auto& depo = dep.second;
	    double offset_ene = depo.deposit-m_adc_offset;
	    ADCValue::value_t adc_count = std::round(((offset_ene) * m_adc_resolution) / m_signal_saturation);
	    adc_count = std::min(adc_count, ADCValue::value_t(m_adc_resolution));
	    response.emplace(cell, {adc_count, ADCValue::address_t(cell)});
	    history.insert(cell, depo.history);
	  }
	}
	info("%s+++ %-32s %6ld ADC values. Input: %-32s %6ld deposits", tag,
	     response_name.c_str(), response.size(), input.name.c_str(), input.size());
        work.output.data.put(response.key, std::move(response));
        work.output.data.put(history.key,  std::move(history));
      }

      /// Create container with ADC counts and register it to the output segment
      template <typename P>
      void emulate_adc(DigiContext& context, work_t& work, const P& predicate)  const  {
	const char* tag = context.event->id();
	if ( const auto* m = work.get_input<DepositMapping>() )
	  emulate_adc(tag, *m, work, predicate);
	else if ( const auto* v = work.get_input<DepositVector>() )
	  emulate_adc(tag, *v, work, predicate);
	else
	  except("%s+++ Request to handle unknown data type: %s",
		 tag, work.input_type_name().c_str());
      }

      /// Main functional callback
      virtual void execute(DigiContext& context, work_t& work, const predicate_t& predicate)  const final  {
	emulate_adc(context, work, predicate);
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep
//        Factory definition
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSimpleADCResponse)

