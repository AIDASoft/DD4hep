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
#ifndef DD4HEP_DDDIGI_DIGIUNIFORMNOISE_H
#define DD4HEP_DDDIGI_DIGIUNIFORMNOISE_H

/// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Generic noise source with a uniform distribution in interval [m_min, m_max]
    /**
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiUniformNoise : public DigiSignalProcessor  {
    protected:
      /// Property: Mean value of the 
      double    m_min     = 0.0;
      /// Property: Variance of the energy distribution in electron Volt. MANDATORY!
      double    m_max     = -1.0;
      
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiUniformNoise);

    public:
      /// Standard constructor
      DigiUniformNoise(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiUniformNoise();
      /// Callback to read event uniformnoise
      virtual double operator()(DigiContext& context, const DigiCellData& data)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIUNIFORMNOISE_H
