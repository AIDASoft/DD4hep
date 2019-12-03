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
#ifndef DD4HEP_DDDIGI_DIGIEXPONENTIALNOISE_H
#define DD4HEP_DDDIGI_DIGIEXPONENTIALNOISE_H

/// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Generic noise source with a exponential distribution with decay to 1/e in tau
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiExponentialNoise : public DigiSignalProcessor  {
    protected:
      /// Property: Exponential decay time
      double    m_tau     = 1.0;
      
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiExponentialNoise);

    public:
      /// Standard constructor
      DigiExponentialNoise(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiExponentialNoise();
      /// Callback to read event exponentialnoise
      virtual double operator()(DigiContext& context, const DigiCellData& data)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIEXPONENTIALNOISE_H
