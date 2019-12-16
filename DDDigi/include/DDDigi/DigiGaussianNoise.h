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
#ifndef DD4HEP_DDDIGI_DIGIGAUSSIANNOISE_H
#define DD4HEP_DDDIGI_DIGIGAUSSIANNOISE_H

/// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Generic noise source with a gaussian distribution
    /**
     *  Generate gaussian noise as it appears e.g. from electronic noise
     *  with a given mean and a given sigma
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiGaussianNoise : public DigiSignalProcessor  {
    protected:
      /// Property: Mean value of the 
      double    m_mean    = 0.0;
      /// Property: Variance of the energy distribution in electron Volt. MANDATORY!
      double    m_sigma   = -1.0;
      /// Property: Cut-off parameter. Do nothing if existing energy deposit is below threshold
      double    m_cutoff  = -1.0;
      
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiGaussianNoise);

    public:
      /// Standard constructor
      DigiGaussianNoise(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiGaussianNoise();
      /// Callback to read event gaussiannoise
      virtual double operator()(DigiCellContext& context)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIGAUSSIANNOISE_H
