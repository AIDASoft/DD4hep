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
#ifndef DD4HEP_DDDIGI_DIGIATTENUATOR_H
#define DD4HEP_DDDIGI_DIGIATTENUATOR_H

/// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// C/C++ include files
#include <functional>

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    /// Base class for attenuator actions to the digitization
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiAttenuator : public DigiSignalProcessor {
    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiAttenuator);
      /// Function object to be executed
      std::function<double(const DigiCellData&)> processor;
    public:
      /// Standard constructor
      DigiAttenuator(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiAttenuator();
      /// Callback to read event attenuator
      virtual double operator()(const DigiCellData& data)  const  override {
        return processor(data);
      }
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGIATTENUATOR_H
