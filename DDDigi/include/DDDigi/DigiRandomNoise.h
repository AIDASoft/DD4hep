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
#ifndef DD4HEP_DDDIGI_DIGIRANDOMNOISE_H
#define DD4HEP_DDDIGI_DIGIRANDOMNOISE_H

/// Framework include files
#include "DDDigi/DigiSignalProcessor.h"
#include "DDDigi/FalphaNoise.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiRandomNoise;

    /// Generic noise source for colored noise: white, pink and brown
    /**
     *  Generic noise source for colored noise: white, pink and brown
     *  Uses internall a 1 / f**alpha noise generator with
     *  alpha = 0  -> white noise
     *  alpha = 1  -> pink  noise (decay 10 db per decade) 
     *  alpha = 2  -> red (brownian) noise  (decay 20 db per decade) 
     *  See https://en.wikipedia.org/wiki/White_noise  for details on colored noise.
     *  See the header FalphaNoise.h and references therein for details 
     *  about the generation.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiRandomNoise : public DigiSignalProcessor  {
    protected:
      /// Property: Alpha parameter of the distribution
      double    m_alpha    = 1.0;
      /// Property: Variance of the energy distribution in electron Volt. MANDATORY!
      double    m_variance = -1;
      /// Property: Number of IRR poles for the noise generator (5 should fit nearly everything)
      double    m_poles    = 5;

      /// Noise generator
      detail::FalphaNoise  m_noise;
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiRandomNoise);

    public:
      /// Standard constructor
      DigiRandomNoise(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiRandomNoise();
      /// Initialize the noise source
      virtual void initialize()  override;
      /// Callback to read event randomnoise
      virtual double operator()(const DigiCellData& data)  const;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIRANDOMNOISE_H
