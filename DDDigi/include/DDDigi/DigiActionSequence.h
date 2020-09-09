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
#ifndef DDDIGI_DIGIACTIONSEQUENCE_H
#define DDDIGI_DIGIACTIONSEQUENCE_H

// Framework include files
#include "DDDigi/DigiSynchronize.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Digitization part of the AIDA detector description toolkit
  namespace digi {

    // Forward declarations
    class DigiEventAction;
    class DigiActionSequence;

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
    class DigiActionSequence : public DigiSynchronize {
    protected:
      /// Callback sequence before the digitization modules execute
      CallbackSequence   m_begin;
      /// Callback sequence after the digitization modules executed
      CallbackSequence   m_end;

    protected:
      /// Define standard assignments and constructors
      DDDIGI_DEFINE_ACTION_CONSTRUCTORS(DigiActionSequence);

    public:
      /// Standard constructor
      DigiActionSequence(const DigiKernel& kernel, const std::string& nam);
      /// Default destructor
      virtual ~DigiActionSequence();
      /// Adopt a new action as part of the sequence. Sequence takes ownership.
      void adopt(DigiEventAction* action);
      /// Register external listener callbacks before starting the sequence
      template <typename Q, typename T>
      void begin(Q* p, void (T::*f)(DigiContext* context)) {
        m_begin.add(p, f);
      }
      /// Register external listener callbacks after finishing the sequence
      template <typename Q, typename T>
      void end(Q* p, void (T::*f)(DigiContext* context))  {
        m_end.add(p, f);
      }
      /// Begin-of-event callback
      virtual void execute(DigiContext& context)  const override;
    };

  }    // End namespace digi
}      // End namespace dd4hep
#endif // DDDIGI_DIGIACTIONSEQUENCE_H
