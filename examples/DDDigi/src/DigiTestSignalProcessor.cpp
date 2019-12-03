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
#ifndef DD4HEP_DDDIGI_DIGITESTSIGNALPROCESSOR_H
#define DD4HEP_DDDIGI_DIGITESTSIGNALPROCESSOR_H

// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiSignalProcessor;
    class DigiTestSignalProcessor;

    /// Class which applies random noise hits of a given amplitude
    /**
     *  Class which applies random noise hits of a given amplitude
     *  to a segmented sensitive element.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiTestSignalProcessor : public DigiSignalProcessor {
    protected:
      double m_attenuation = 1.0;
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiTestSignalProcessor);
      
    public:
      /// Standard constructor
      DigiTestSignalProcessor(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiTestSignalProcessor();
      /// Process signal data
      virtual double operator()(DigiContext& context, const DigiCellData& data)   const  override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGITESTSIGNALPROCESSOR_H

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
#include "DD4hep/Printout.h"
#include "DD4hep/InstanceCount.h"
#include "DDDigi/DigiFactories.h"
#include "DDDigi/DigiSegmentation.h"
//#include "DDDigi/DigiTestSignalProcessor.h"

// C/C++ include files

using namespace std;
using namespace dd4hep::digi;

DECLARE_DIGISIGNALPROCESSOR_NS(dd4hep::digi,DigiTestSignalProcessor)

/// Standard constructor
DigiTestSignalProcessor::DigiTestSignalProcessor(const DigiKernel& kernel, const string& nam)
  : DigiSignalProcessor(kernel, nam)
{
  declareProperty("attenuation", m_attenuation = 1.0);
  InstanceCount::increment(this);
}

/// Default destructor
DigiTestSignalProcessor::~DigiTestSignalProcessor() {
  InstanceCount::decrement(this);
}

/// Process signal data
double DigiTestSignalProcessor::operator()(DigiContext&, const DigiCellData& data)   const   {
  return m_attenuation * data.signal;
}
