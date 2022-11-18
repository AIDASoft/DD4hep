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

    class DigiSimpleADCResponse : public DigiDepositsProcessor  {
      using segmentation_t = DigiSegmentProcessContext;
      std::string    m_response_postfix   { ".adc" };
      std::string    m_history_postfix    { ".hist" };
      double         m_signal_saturation  { std::numeric_limits<double>::max() };
      double         m_adc_offset         { 0e0  };
      std::size_t    m_adc_resolution     { 1024 };
      
      public:
      /// Standard constructor
      DigiSimpleADCResponse(const DigiKernel& krnl, const std::string& nam)
	: DigiDepositsProcessor(krnl, nam)
      {
	declareProperty("saturation",       m_signal_saturation);
	declareProperty("adc_resolution",   m_adc_resolution);
	declareProperty("response_postfix", m_response_postfix);
	declareProperty("history_postfix",  m_history_postfix);
	DEPOSIT_PROCESSOR_BIND_HANDLERS(DigiSimpleADCResponse::emulate_adc)
      }

      /// Create container with ADC counts and register it to the output segment
      template <typename T>
      void emulate_adc(DigiContext& context, const T& input, work_t& work, const predicate_t& predicate)  const  {
	const char* tag = context.event->id();
	std::string postfix = predicate.segmentation ? "."+predicate.segmentation->identifier(predicate.id) : std::string();
	std::string response_name = input.name + postfix + m_response_postfix;
	DetectorResponse response(response_name, work.environ.output.mask);
	for( const auto& dep : input )   {
	  if ( predicate(dep) )   {
	    CellID      cell = dep.first;
	    const auto& depo = dep.second;
	    double offset_ene = depo.deposit-m_adc_offset;
	    ADCValue::value_t adc_count = std::round(((offset_ene) * m_adc_resolution) / m_signal_saturation);
	    adc_count = std::min(adc_count, ADCValue::value_t(m_adc_resolution));
	    response.emplace(cell, {adc_count, ADCValue::address_t(cell)});
	  }
	}
#ifdef DDDIGI_INPLACE_HISTORY
	std::string history_name  = input.name + postfix + m_history_postfix;
	DetectorHistory  history (history_name,  work.output.mask);
	for( const auto& dep : input )   {
	  if ( predicate(dep) )   {
	    history.insert(dep.first, dep.second.history);
	  }
	}
        work.output.data.put(history.key,  std::move(history));
#endif
	info("%s+++ %-32s %6ld ADC values. Input: %-32s %6ld deposits", tag,
	     response_name.c_str(), response.size(), input.name.c_str(), input.size());
        work.environ.output.data.put(response.key, std::move(response));
      }
    };
  }    // End namespace digi
}      // End namespace dd4hep
//        Factory definition
#include <DDDigi/DigiFactories.h>
DECLARE_DIGIACTION_NS(dd4hep::digi,DigiSimpleADCResponse)

