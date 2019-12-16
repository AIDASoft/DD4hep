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
#ifndef DD4HEP_DDDIGI_DIGISIGNALPROCESSORSEQUENCE_H
#define DD4HEP_DDDIGI_DIGISIGNALPROCESSORSEQUENCE_H

// Framework include files
#include "DDDigi/DigiSignalProcessor.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiSignalProcessor;
    class DigiSignalProcessorSequence;

    /// Concrete implementation of the Digitization event action sequence
    /**
     * The sequence dispatches the callbacks to all registered DigiAction 
     * members and all registered callbacks.
     *
     * Note Multi-Threading issue:
     * Neither callbacks not the action list is protected against multiple 
     * threads calling the Digi callbacks!
     * These must be protected in the user actions themselves.
     *
     *  \author  M.Frank
     *  \version 1.0
     *  \ingroup DD4HEP_DIGITIZATION
     */
    class DigiSignalProcessorSequence : public DigiSignalProcessor {
    protected:
      /// The list of action objects to be called
      Actors<DigiSignalProcessor> m_actors;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiSignalProcessorSequence);

    public:
      /// Standard constructor
      DigiSignalProcessorSequence(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiSignalProcessorSequence();
      /// Adopt a new action as part of the sequence. Sequence takes ownership.
      void adopt(DigiSignalProcessor* action);
      /// Begin-of-event callback
      virtual double operator()(DigiCellContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DD4HEP_DDDIGI_DIGISIGNALPROCESSORSEQUENCE_H
