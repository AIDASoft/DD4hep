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

// Framework include files
#include "DDDigi/DigiAction.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiAction;
    class DigiRandomNoise;

    /// Class which applies random noise hits of a given amplitude
    /**
     *  Class which applies random noise hits of a given amplitude
     *  to a segmented sensitive element.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiRandomNoise : public DigiAction {
    protected:
      double m_probability = 1.0;
      double m_amplitude   = 1.0;
      
    protected:
      /// Inhibit copy constructor
      DigiRandomNoise() = default;
      /// Inhibit copy constructor
      DigiRandomNoise(const DigiRandomNoise& copy) = delete;
      /// Inhibit assignment operator
      DigiRandomNoise& operator=(const DigiRandomNoise& copy) = delete;

    public:
      /// Standard constructor
      DigiRandomNoise(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiRandomNoise();
      /// Callback to read event input
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIRANDOMNOISE_H

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
//#include "DDDigi/DigiRandomNoise.h"

// C/C++ include files

using namespace std;
using namespace dd4hep::digi;

DECLARE_DIGIACTION_NS(dd4hep::digi,DigiRandomNoise)

/// Standard constructor
DigiRandomNoise::DigiRandomNoise(const DigiKernel& kernel, const string& nam)
  : DigiAction(kernel, nam)
{
  declareProperty("Probability", m_probability);
  declareProperty("Amplitude",   m_amplitude);
  InstanceCount::increment(this);
}

/// Default destructor
DigiRandomNoise::~DigiRandomNoise() {
  InstanceCount::decrement(this);
}

/// Pre-track action callback
void DigiRandomNoise::execute(DigiContext& /* context */)  const   {
  info("+++ Virtual method execute() MUST be overloaded!");
}
