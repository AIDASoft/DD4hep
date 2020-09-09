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
#ifndef DDDIGI_DIGIPOISSONNOISE_H
#define DDDIGI_DIGIPOISSONNOISE_H

/// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Generic noise source with a poisson distribution
    /**
     *  Generate poisson noise as it appears e.g. from electronic noise
     *  with a given mean and a given sigma
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiPoissonNoise : public DigiSignalProcessor  {
    protected:
      /// Property: Mean value of the 
      double    m_mean    = 0.0;
      /// Property: Cut-off parameter. Do nothing if existing energy deposit is above threshold
      double    m_cutoff  = -1.0;
      
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiPoissonNoise);

    public:
      /// Standard constructor
      DigiPoissonNoise(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiPoissonNoise();
      /// Callback to read event poissonnoise
      virtual double operator()(DigiCellContext& context)  const  override;
    };
  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIPOISSONNOISE_H
